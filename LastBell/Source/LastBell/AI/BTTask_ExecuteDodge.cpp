#include "AI/BTTask_ExecuteDodge.h"
#include "Characters/AIBoxer.h"
#include "AIController.h"

UBTTask_ExecuteDodge::UBTTask_ExecuteDodge()
{
    NodeName = "Execute Dodge";
}

EBTNodeResult::Type UBTTask_ExecuteDodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (FMath::FRand() > DodgeProbability) return EBTNodeResult::Failed;

    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    AAIBoxer* Boxer = Cast<AAIBoxer>(Controller->GetPawn());
    if (!Boxer) return EBTNodeResult::Failed;

    Boxer->AIExecuteDodge();
    return EBTNodeResult::Succeeded;
}
