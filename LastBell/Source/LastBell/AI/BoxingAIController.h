#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BoxingAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;

namespace BBKeys
{
    const FName Player      = TEXT("Player");
    const FName Self        = TEXT("Self");
    const FName DistToPlayer = TEXT("DistToPlayer");
    const FName bCanAttack  = TEXT("bCanAttack");
    const FName bPlayerAttacking = TEXT("bPlayerAttacking");
    const FName AttackMove  = TEXT("AttackMove");
    const FName HealthPercent = TEXT("HealthPercent");
    const FName bShouldRetreat = TEXT("bShouldRetreat");
    const FName AggressionState = TEXT("AggressionState");
    const FName FatigueLevel = TEXT("FatigueLevel");
    const FName bInComebackMode = TEXT("bInComebackMode");
}

UCLASS()
class LASTBELL_API ABoxingAIController : public AAIController
{
    GENERATED_BODY()

public:
    ABoxingAIController();

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void StartAI(AActor* PlayerActor);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void StopAI();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

private:
    FTimerHandle UpdateTimer;

    void UpdateBlackboard();
};
