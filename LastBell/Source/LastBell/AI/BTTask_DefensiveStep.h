#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DefensiveStep.generated.h"

UCLASS()
class LASTBELL_API UBTTask_DefensiveStep : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_DefensiveStep();

    UPROPERTY(EditAnywhere, Category = "Defense")
    float StepDistance = 120.f;

    UPROPERTY(EditAnywhere, Category = "Defense")
    bool bRandomDirection = true;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
