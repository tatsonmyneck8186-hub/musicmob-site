#pragma once

#include "CoreMinimal.h"
#include "Characters/BoxerCharacter.h"
#include "AIBoxer.generated.h"

class UBoxingAIPersonalityComponent;

UCLASS()
class LASTBELL_API AAIBoxer : public ABoxerCharacter
{
    GENERATED_BODY()

public:
    AAIBoxer();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxingAIPersonalityComponent> PersonalityComponent;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AIExecuteAttack(EBoxingMove Move);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AIExecuteDodge();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AIMoveTowardPlayer(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AIMoveAwayFromPlayer(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "AI")
    float GetDistanceToOpponent() const;

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsOpponentAttacking() const;

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsInAttackRange() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange = 150.f;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHealthChanged(float NewHealth, float MaxHealth);
};
