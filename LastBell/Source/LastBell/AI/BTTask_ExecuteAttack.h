#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Core/BoxingTypes.h"
#include "BTTask_ExecuteAttack.generated.h"

UCLASS()
class LASTBELL_API UBTTask_ExecuteAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ExecuteAttack();

    UPROPERTY(EditAnywhere, Category = "Attack")
    bool bPickRandomMove = true;

    UPROPERTY(EditAnywhere, Category = "Attack", meta=(EditCondition="bPickRandomMove"))
    FVector MoveWeights = FVector(0.5f, 0.3f, 0.2f);

    UPROPERTY(EditAnywhere, Category = "Attack", meta=(EditCondition="!bPickRandomMove"))
    EBoxingMove SpecificMove = EBoxingMove::Jab;

    UPROPERTY(EditAnywhere, Category = "Attack")
    FBlackboardKeySelector AttackMoveKey;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
