#include "AI/BTTask_DefensiveStep.h"
#include "Characters/AIBoxer.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_DefensiveStep::UBTTask_DefensiveStep()
{
    NodeName = "Defensive Step";
}

EBTNodeResult::Type UBTTask_DefensiveStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    ACharacter* Boxer = Cast<ACharacter>(Controller->GetPawn());
    if (!Boxer) return EBTNodeResult::Failed;

    float Direction = bRandomDirection ? (FMath::RandBool() ? 1.f : -1.f) : 1.f;
    FVector RightVec = Boxer->GetActorRightVector() * Direction * StepDistance;

    Boxer->LaunchCharacter(RightVec * 3.f, true, false);

    return EBTNodeResult::Succeeded;
}
