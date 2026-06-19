#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveTowardPlayer.generated.h"

UCLASS()
class LASTBELL_API UBTTask_MoveTowardPlayer : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_MoveTowardPlayer();

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bMoveAway = false;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveDuration = 0.5f;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
