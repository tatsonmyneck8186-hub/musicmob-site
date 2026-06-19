#include "AI/BoxingAIController.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxerStatsComponent.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "Core/IBoxerInterface.h"
#include "Data/FighterDataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ABoxingAIController::ABoxingAIController()
{
    bWantsPlayerState = false;
}

void ABoxingAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ABoxingAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTree)
    {
        bUseSimpleAI = false;
        RunBehaviorTree(BehaviorTree);
    }
    else
    {
        // No authored Behavior Tree — fall back to the built-in C++ fight loop so
        // the game is fully playable from code alone.
        bUseSimpleAI = true;
    }
}

void ABoxingAIController::StartAI(AActor* PlayerActor)
{
    PlayerRef = PlayerActor;

    if (bUseSimpleAI)
    {
        GetWorldTimerManager().SetTimer(SimpleThinkTimer, this,
            &ABoxingAIController::SimpleThink, 0.35f, true);
        return;
    }

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsObject(BBKeys::Player, PlayerActor);
        BB->SetValueAsObject(BBKeys::Self, GetPawn());
    }

    GetWorldTimerManager().SetTimer(UpdateTimer, this,
        &ABoxingAIController::UpdateBlackboard, 0.1f, true);
}

void ABoxingAIController::StopAI()
{
    GetWorldTimerManager().ClearTimer(UpdateTimer);
    GetWorldTimerManager().ClearTimer(SimpleThinkTimer);
    if (AAIBoxer* Boxer = Cast<AAIBoxer>(GetPawn()))
    {
        Boxer->SetApproachIntent(0.f);
    }
    StopMovement();
}

void ABoxingAIController::SimpleThink()
{
    AAIBoxer* Boxer = Cast<AAIBoxer>(GetPawn());
    if (!Boxer) return;

    const EBoxerState State = Boxer->GetCurrentState();
    const bool bBusy = State == EBoxerState::Attacking || State == EBoxerState::HitStun ||
                       State == EBoxerState::KnockedDown || State == EBoxerState::KO ||
                       State == EBoxerState::Dodging;
    if (bBusy)
    {
        Boxer->SetApproachIntent(0.f);
        return;
    }

    UBoxingAIPersonalityComponent* Personality = Boxer->PersonalityComponent;
    const float HealthPct = Boxer->StatsComponent ? Boxer->StatsComponent->GetHealthPercent() : 1.f;
    const float Dist = Boxer->GetDistanceToOpponent();

    // Defensive reaction: slip an incoming punch.
    const float DodgeChance = Personality ? Personality->GetDodgeProbability() : 0.2f;
    if (Boxer->IsOpponentAttacking() && Dist < Boxer->AttackRange * 1.4f && FMath::FRand() < DodgeChance)
    {
        Boxer->SetApproachIntent(0.f);
        Boxer->AIExecuteDodge();
        return;
    }

    // Retreat briefly when hurt and not in comeback mode.
    const bool bComeback = Personality && Personality->IsInComebackMode();
    if (HealthPct < 0.18f && !bComeback && FMath::FRand() < 0.4f)
    {
        Boxer->SetApproachIntent(-1.f);
        return;
    }

    if (Dist <= Boxer->AttackRange)
    {
        Boxer->SetApproachIntent(0.f);

        // Throw on a cadence scaled by aggression / fatigue.
        float AttackChance = 0.5f;
        if (Personality)
        {
            AttackChance = 0.35f * Personality->GetAttackFrequencyMultiplier();
            if (bComeback) AttackChance += 0.2f;
        }

        if (FMath::FRand() < AttackChance)
        {
            FVector W = Boxer->GetFighterData() ? Boxer->GetFighterData()->Personality.MoveWeights : FVector(0.6f, 0.3f, 0.1f);
            const float Total = FMath::Max(0.01f, W.X + W.Y + W.Z);
            const float Roll = FMath::FRandRange(0.f, Total);
            EBoxingMove Move = EBoxingMove::Jab;
            if (Roll > W.X + W.Y) Move = EBoxingMove::Uppercut;
            else if (Roll > W.X)  Move = EBoxingMove::Hook;
            Boxer->AIExecuteAttack(Move);
        }
    }
    else
    {
        // Close the distance.
        Boxer->SetApproachIntent(1.f);
    }
}

void ABoxingAIController::UpdateBlackboard()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    AAIBoxer* ControlledBoxer = Cast<AAIBoxer>(GetPawn());
    if (!ControlledBoxer) return;

    AActor* Player = Cast<AActor>(BB->GetValueAsObject(BBKeys::Player));
    if (!Player) return;

    float Dist = ControlledBoxer->GetDistanceToOpponent();
    BB->SetValueAsFloat(BBKeys::DistToPlayer, Dist);

    EBoxerState State = ControlledBoxer->GetCurrentState();
    bool bCanAct = State != EBoxerState::Attacking &&
                   State != EBoxerState::HitStun &&
                   State != EBoxerState::KnockedDown &&
                   State != EBoxerState::KO;
    BB->SetValueAsBool(BBKeys::bCanAttack, bCanAct);

    bool bPlayerIsAttacking = false;
    if (Player->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        bPlayerIsAttacking = IBoxerInterface::Execute_IsAttacking(Player);
    }
    BB->SetValueAsBool(BBKeys::bPlayerAttacking, bPlayerIsAttacking);

    float HealthPct = ControlledBoxer->StatsComponent->GetHealthPercent();
    BB->SetValueAsFloat(BBKeys::HealthPercent, HealthPct);
    BB->SetValueAsBool(BBKeys::bShouldRetreat, HealthPct < 0.2f);

    UBoxingAIPersonalityComponent* Personality = ControlledBoxer->PersonalityComponent;
    if (Personality)
    {
        BB->SetValueAsEnum(BBKeys::AggressionState,
            (uint8)Personality->GetAggressionState());
        BB->SetValueAsFloat(BBKeys::FatigueLevel, Personality->GetFatigueLevel());
        BB->SetValueAsBool(BBKeys::bInComebackMode, Personality->IsInComebackMode());
    }
}
