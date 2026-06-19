#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Core/BoxingTypes.h"
#include "BTTask_FakePunch.generated.h"

UCLASS()
class LASTBELL_API UBTTask_FakePunch : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FakePunch();

    UPROPERTY(EditAnywhere, Category = "FakePunch")
    EBoxingMove FakeMove = EBoxingMove::Jab;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
