#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Core/BoxingTypes.h"
#include "BoxerAnimInstance.generated.h"

class ABoxerCharacter;

UCLASS()
class LASTBELL_API UBoxerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "State")
    EBoxerState BoxerState = EBoxerState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    EBoxingMove CurrentAttack = EBoxingMove::None;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    EAttackPhase AttackPhase = EAttackPhase::None;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float MoveSpeed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float HorizontalMovement = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float LeanDirection = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsAttacking = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsDucking = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsDodging = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsKnockedDown = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsKO = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bInGuard = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bInStagger = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bGetUp = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bVictory = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bDefeat = false;

    UPROPERTY(BlueprintReadOnly, Category = "Breathing")
    float BreathingWeight = 0.f;

protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    TWeakObjectPtr<ABoxerCharacter> OwnerBoxer;
};
