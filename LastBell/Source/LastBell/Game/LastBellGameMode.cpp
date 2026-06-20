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

    // Wire knockout / knockdown into the match flow. AddUnique so repeated
    // StartMatch calls (rematch) never double-bind.
    if (PlayerBoxer && PlayerBoxer->StatsComponent)
    {
        PlayerBoxer->StatsComponent->OnKO.AddUniqueDynamic(this, &ALastBellGameMode::OnPlayerKO);
        PlayerBoxer->StatsComponent->OnKnockdown.AddUniqueDynamic(this, &ALastBellGameMode::OnPlayerKnockdown);
    }
    if (AIBoxerRef && AIBoxerRef->StatsComponent)
    {
        AIBoxerRef->StatsComponent->OnKO.AddUniqueDynamic(this, &ALastBellGameMode::OnAIKO);
        AIBoxerRef->StatsComponent->OnKnockdown.AddUniqueDynamic(this, &ALastBellGameMode::OnAIKnockdown);
    }

    CurrentRound = 0;
    PlayerRoundWins = 0;
    AIRoundWins = 0;
    bMatchPlayerWon = false;
    bResolving = false;
    SetMatchState(EMatchState::WaitingToStart);

    GetWorldTimerManager().SetTimer(SequenceTimerHandle, [this]()
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

    if (PlayerBoxer) { PlayerBoxer->ResetState(); PlayerBoxer->StatsComponent->ResetForNewRound(); }
    if (AIBoxerRef)  { AIBoxerRef->ResetState();  AIBoxerRef->StatsComponent->ResetForNewRound(); }

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
    // Guard against a stray/second end (e.g. a buffered timer after a KO already resolved).
    if (CurrentMatchState != EMatchState::RoundActive && CurrentMatchState != EMatchState::Knockdown) return;

    GetWorldTimerManager().ClearTimer(RoundTimerHandle);
    GetWorldTimerManager().ClearTimer(KnockdownTimerHandle);
    SetMatchState(EMatchState::BetweenRounds);

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);

    if (bPlayerWon) PlayerRoundWins++;
    else            AIRoundWins++;

    OnRoundEnd.Broadcast(bPlayerWon, bByKnockdown);

    bool bMatchDecided = (PlayerRoundWins > TotalRounds / 2 || AIRoundWins > TotalRounds / 2);

    if (CurrentRound >= TotalRounds || bMatchDecided)
    {
        GetWorldTimerManager().SetTimer(SequenceTimerHandle, [this]()
        {
            FinalizeMatch();
        }, 3.f, false);
    }
    else
    {
        GetWorldTimerManager().SetTimer(SequenceTimerHandle, [this]()
        {
            StartRound();
        }, 5.f, false);
    }
}

void ALastBellGameMode::OnPlayerKO() { ResolveKO(true); }
void ALastBellGameMode::OnAIKO()     { ResolveKO(false); }
void ALastBellGameMode::OnPlayerKnockdown() { TriggerKnockdown(PlayerBoxer); }
void ALastBellGameMode::OnAIKnockdown()     { TriggerKnockdown(AIBoxerRef); }

void ALastBellGameMode::ResolveKO(bool bPlayerWasKOd)
{
    // A health-zero knockout ends the whole match immediately.
    if (bResolving) return;
    if (CurrentMatchState != EMatchState::RoundActive && CurrentMatchState != EMatchState::Knockdown) return;
    bResolving = true;

    GetWorldTimerManager().ClearTimer(RoundTimerHandle);
    GetWorldTimerManager().ClearTimer(KnockdownTimerHandle);
    GetWorldTimerManager().ClearTimer(SequenceTimerHandle);

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);

    if (bPlayerWasKOd) AIRoundWins = TotalRounds;
    else               PlayerRoundWins = TotalRounds;

    SetMatchState(EMatchState::MatchOver);

    GetWorldTimerManager().SetTimer(SequenceTimerHandle, [this]()
    {
        FinalizeMatch();
    }, 2.5f, false);
}

void ALastBellGameMode::TriggerKnockdown(ABoxerCharacter* KnockedDownFighter)
{
    if (!KnockedDownFighter) return;
    if (CurrentMatchState != EMatchState::RoundActive) return;

    // Stop the round clock so it can't expire (and race EndRound) during the count.
    GetWorldTimerManager().ClearTimer(RoundTimerHandle);

    SetMatchState(EMatchState::Knockdown);
    KnockdownCount = 0;

    // Freeze both fighters for the count.
    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);

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
    bMatchPlayerWon = (PlayerRoundWins > AIRoundWins);
    SetMatchState(EMatchState::MatchOver);
    OnMatchEnd.Broadcast(bMatchPlayerWon);
    HandleMatchEnd(bMatchPlayerWon);
}

void ALastBellGameMode::HandleMatchEnd(bool bPlayerWon)
{
    // Default (authored-map) path: transition to the win/lose level after a beat.
    // The arena (code-only) game mode overrides this to stay in-world.
    FString MapName = bPlayerWon ? TEXT("/Game/LastBell/Maps/L_WinScreen") : TEXT("/Game/LastBell/Maps/L_LoseScreen");
    GetWorldTimerManager().SetTimer(SequenceTimerHandle, [this, MapName]()
    {
        UGameplayStatics::OpenLevel(this, FName(*MapName));
    }, 3.f, false);
}

void ALastBellGameMode::AbortMatch()
{
    GetWorldTimerManager().ClearTimer(RoundTimerHandle);
    GetWorldTimerManager().ClearTimer(SequenceTimerHandle);
    GetWorldTimerManager().ClearTimer(KnockdownTimerHandle);
    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);
    CurrentRound = 0;
    bResolving = false;
    // Defensive: never leave a fight (e.g. bailing mid-KO-slowmo) with altered time.
    UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
    SetMatchState(EMatchState::WaitingToStart);
}

void ALastBellGameMode::SetMatchState(EMatchState NewState)
{
    CurrentMatchState = NewState;
    OnMatchStateChanged.Broadcast(NewState);
}
