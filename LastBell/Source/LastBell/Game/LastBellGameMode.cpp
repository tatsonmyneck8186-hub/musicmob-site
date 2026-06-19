#include "Game/LastBellGameMode.h"
#include "Characters/BoxerCharacter.h"
#include "Characters/PlayerBoxer.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxerFeedbackComponent.h"
#include "Components/BoxerStatsComponent.h"
#include "Data/FighterDataAsset.h"
#include "AI/BoxingAIController.h"
#include "Core/IBoxerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ALastBellGameMode::ALastBellGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALastBellGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBellGameMode::StartMatch(UFighterDataAsset* OpponentData)
{
    TArray<AActor*> Boxers;
    UGameplayStatics::GetAllActorsOfClass(this, ABoxerCharacter::StaticClass(), Boxers);

    for (AActor* Actor : Boxers)
    {
        ABoxerCharacter* Boxer = Cast<ABoxerCharacter>(Actor);
        if (!Boxer) continue;

        if (Cast<APlayerBoxer>(Boxer))
        {
            PlayerBoxer = Boxer;
        }
        else if (AAIBoxer* AI = Cast<AAIBoxer>(Boxer))
        {
            AIBoxerRef = Boxer;
            if (OpponentData) Boxer->LoadFighterData(OpponentData);

            if (ABoxingAIController* AIC = Cast<ABoxingAIController>(AI->GetController()))
            {
                AIC->StartAI(PlayerBoxer);
            }
        }
    }

    if (PlayerBoxer && AIBoxerRef)
    {
        IBoxerInterface::Execute_SetOpponent(PlayerBoxer, AIBoxerRef);
        IBoxerInterface::Execute_SetOpponent(AIBoxerRef, PlayerBoxer);
    }

    CurrentRound = 0;
    PlayerRoundWins = 0;
    AIRoundWins = 0;

    GetWorldTimerManager().SetTimer(RoundTimerHandle, [this]()
    {
        StartRound();
    }, 3.f, false);
}

void ALastBellGameMode::StartRound()
{
    CurrentRound++;
    RoundTimeRemaining = RoundDuration;
    SetMatchState(EMatchState::RoundActive);
    OnRoundChanged.Broadcast(CurrentRound, TotalRounds);

    if (PlayerBoxer) PlayerBoxer->StatsComponent->ResetForNewRound();
    if (AIBoxerRef)  AIBoxerRef->StatsComponent->ResetForNewRound();

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(true);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(true);

    GetWorldTimerManager().SetTimer(RoundTimerHandle, this,
        &ALastBellGameMode::TickRoundTimer, 1.f, true);
}

void ALastBellGameMode::TickRoundTimer()
{
    RoundTimeRemaining -= 1.f;
    OnTimerTick.Broadcast(RoundTimeRemaining);

    if (RoundTimeRemaining <= 0.f)
    {
        GetWorldTimerManager().ClearTimer(RoundTimerHandle);

        float PlayerHP = PlayerBoxer ? PlayerBoxer->GetHealthPercent_Implementation() : 0.f;
        float AIHP    = AIBoxerRef  ? AIBoxerRef->GetHealthPercent_Implementation() : 0.f;
        bool bPlayerWonRound = (PlayerHP >= AIHP);
        EndRound(bPlayerWonRound, false);
    }
}

void ALastBellGameMode::EndRound(bool bPlayerWon, bool bByKnockdown)
{
    GetWorldTimerManager().ClearTimer(RoundTimerHandle);
    SetMatchState(EMatchState::BetweenRounds);

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);

    if (bPlayerWon) PlayerRoundWins++;
    else            AIRoundWins++;

    OnRoundEnd.Broadcast(bPlayerWon, bByKnockdown);

    bool bMatchDecided = (PlayerRoundWins > TotalRounds / 2 || AIRoundWins > TotalRounds / 2);

    if (CurrentRound >= TotalRounds || bMatchDecided)
    {
        GetWorldTimerManager().SetTimer(RoundTimerHandle, [this]()
        {
            FinalizeMatch();
        }, 3.f, false);
    }
    else
    {
        GetWorldTimerManager().SetTimer(RoundTimerHandle, [this]()
        {
            StartRound();
        }, 5.f, false);
    }
}

void ALastBellGameMode::TriggerKnockdown(ABoxerCharacter* KnockedDownFighter)
{
    if (!KnockedDownFighter) return;

    SetMatchState(EMatchState::Knockdown);
    KnockdownCount = 0;
    KnockedDownFighter->SetBoxerEnabled(false);

    if (AIBoxerRef && Cast<AAIBoxer>(AIBoxerRef))
    {
        AIBoxerRef->SetBoxerEnabled(false);
    }

    UBoxerFeedbackComponent* Feedback = KnockedDownFighter->FeedbackComponent;
    if (Feedback)
    {
        Feedback->TriggerKOSlowMotion(0.3f, 0.15f, 0.5f, 0.35f);
    }

    GetWorldTimerManager().SetTimer(KnockdownTimerHandle, this,
        &ALastBellGameMode::KnockdownCountTick, 1.f, true);
}

void ALastBellGameMode::KnockdownCountTick()
{
    KnockdownCount++;
    OnKnockdownCount.Broadcast(KnockdownCount);

    if (KnockdownCount >= 10)
    {
        GetWorldTimerManager().ClearTimer(KnockdownTimerHandle);

        bool bAIWasKnockedDown = (AIBoxerRef && AIBoxerRef->GetCurrentState() == EBoxerState::KnockedDown);
        EndRound(bAIWasKnockedDown, true);
    }
}

void ALastBellGameMode::FinalizeMatch()
{
    bool bPlayerWon = (PlayerRoundWins > AIRoundWins);
    SetMatchState(EMatchState::MatchOver);
    OnMatchEnd.Broadcast(bPlayerWon);

    FString MapName = bPlayerWon ? TEXT("/Game/LastBell/Maps/L_WinScreen") : TEXT("/Game/LastBell/Maps/L_LoseScreen");
    GetWorldTimerManager().SetTimer(RoundTimerHandle, [this, MapName]()
    {
        UGameplayStatics::OpenLevel(this, FName(*MapName));
    }, 3.f, false);
}

void ALastBellGameMode::SetMatchState(EMatchState NewState)
{
    CurrentMatchState = NewState;
    OnMatchStateChanged.Broadcast(NewState);
}
