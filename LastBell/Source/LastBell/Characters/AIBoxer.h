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

    /** Desired spacing from the opponent the simple AI tries to hold while not advancing. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PreferredSpacing = 130.f;

    /**
     * Per-frame approach intent set by the controller: +1 advance, -1 retreat, 0 hold.
     * Applied in Tick so movement is smooth regardless of the controller's think rate.
     */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetApproachIntent(float Intent) { ApproachIntent = FMath::Clamp(Intent, -1.f, 1.f); }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnHealthChanged(float NewHealth, float MaxHealth);

private:
    float ApproachIntent = 0.f;
};
