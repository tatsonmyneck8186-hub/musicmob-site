#include "AI/BTTask_ExecuteAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AIBoxer.h"
#include "AIController.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
    NodeName = "Execute Attack";
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    AAIBoxer* Boxer = Cast<AAIBoxer>(Controller->GetPawn());
    if (!Boxer) return EBTNodeResult::Failed;

    EBoxingMove MoveToUse = SpecificMove;

    if (bPickRandomMove)
    {
        float Total = MoveWeights.X + MoveWeights.Y + MoveWeights.Z;
        float Rand = FMath::FRandRange(0.f, Total);

        if (Rand < MoveWeights.X) MoveToUse = EBoxingMove::Jab;
        else if (Rand < MoveWeights.X + MoveWeights.Y) MoveToUse = EBoxingMove::Hook;
        else MoveToUse = EBoxingMove::Uppercut;
    }

    Boxer->AIExecuteAttack(MoveToUse);
    return EBTNodeResult::Succeeded;
}
