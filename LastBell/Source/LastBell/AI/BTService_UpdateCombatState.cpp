#include "AI/BTService_UpdateCombatState.h"
#include "AI/BoxingAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxerStatsComponent.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "Core/IBoxerInterface.h"

UBTService_UpdateCombatState::UBTService_UpdateCombatState()
{
    NodeName = "Update Combat State";
    Interval = 0.1f;
    RandomDeviation = 0.f;
}

void UBTService_UpdateCombatState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return;

    AAIBoxer* Boxer = Cast<AAIBoxer>(Controller->GetPawn());
    if (!Boxer) return;

    AActor* Player = Cast<AActor>(BB->GetValueAsObject(BBKeys::Player));

    float Dist = Boxer->GetDistanceToOpponent();
    BB->SetValueAsFloat(BBKeys::DistToPlayer, Dist);

    EBoxerState State = Boxer->GetCurrentState();
    bool bCanAct = State != EBoxerState::Attacking &&
                   State != EBoxerState::HitStun &&
                   State != EBoxerState::KnockedDown &&
                   State != EBoxerState::KO;
    BB->SetValueAsBool(BBKeys::bCanAttack, bCanAct);

    if (Player && Player->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        BB->SetValueAsBool(BBKeys::bPlayerAttacking, IBoxerInterface::Execute_IsAttacking(Player));
    }

    float HealthPct = Boxer->StatsComponent->GetHealthPercent();
    BB->SetValueAsFloat(BBKeys::HealthPercent, HealthPct);
    BB->SetValueAsBool(BBKeys::bShouldRetreat, HealthPct < 0.2f);

    if (Boxer->PersonalityComponent)
    {
        BB->SetValueAsEnum(BBKeys::AggressionState,
            (uint8)Boxer->PersonalityComponent->GetAggressionState());
        BB->SetValueAsFloat(BBKeys::FatigueLevel,
            Boxer->PersonalityComponent->GetFatigueLevel());
        BB->SetValueAsBool(BBKeys::bInComebackMode,
            Boxer->PersonalityComponent->IsInComebackMode());
    }
}
