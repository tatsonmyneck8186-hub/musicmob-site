#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BoxingTypes.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackExecuted, EBoxingMove, Move);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitLanded, AActor*, Target, FAttackData, AttackData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStateChanged, EAttackPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitRegistered, EBoxingMove, Move);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissRegistered, EBoxingMove, Move);

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAttackExecuted OnAttackExecuted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHitLanded OnHitLanded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAttackStateChanged OnAttackStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHitRegistered OnHitRegistered;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMissRegistered OnMissRegistered;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TryAttack(EBoxingMove Move, const FAttackData& AttackData);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TryFakePunch(EBoxingMove Move, const FAttackData& AttackData);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryDodge();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsAttacking() const { return bIsAttacking; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsDodging() const { return bIsDodging; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool CanStartAttack() const { return bCombatEnabled && !bIsAttacking && !bIsDodging; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsCombatEnabled() const { return bCombatEnabled; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    EAttackPhase GetCurrentAttackPhase() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetOpponent(AActor* InOpponent) { Opponent = InOpponent; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetEnabled(bool bEnabled) { bCombatEnabled = bEnabled; }

    bool bIsDucking = false;
    float DodgeDuration = 0.3f;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool bIsAttacking = false;
    bool bIsDodging = false;
    bool bCombatEnabled = true;
    bool bHitRegisteredThisAttack = false;
    bool bIsFakeAttack = false;

    float AttackElapsed = 0.f;
    float HitboxActiveStart = 0.f;
    float HitboxActiveEnd = 0.f;
    float AttackTotalDuration = 0.f;
    float DodgeElapsed = 0.f;

    bool bInHitPause = false;
    double HitPauseStartRealTime = 0.0;
    float HitPauseRealDuration = 0.f;

    FAttackData CurrentAttackData;
    TWeakObjectPtr<AActor> Opponent;

    void TickAttack(float DeltaTime);
    void TickDodge(float DeltaTime);
    void TickHitPause();
    void CheckHit();
    void EndAttack();
    void StartHitPause(float RealDuration);

    bool IsOpponentInRange() const;
    bool IsOpponentInAngle() const;
    bool CanOpponentEvade(EBoxingMove Move) const;
};
