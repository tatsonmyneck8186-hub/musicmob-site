#include "AI/BoxingAIController.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxerStatsComponent.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "Core/IBoxerInterface.h"
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
        RunBehaviorTree(BehaviorTree);
    }
}

void ABoxingAIController::StartAI(AActor* PlayerActor)
{
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
    StopMovement();
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
