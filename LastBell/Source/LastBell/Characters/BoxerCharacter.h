#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/IBoxerInterface.h"
#include "BoxerCharacter.generated.h"

class UBoxerStatsComponent;
class UCombatComponent;
class UBoxerFeedbackComponent;
class UComboComponent;
class UBoxerAudioComponent;
class UFighterDataAsset;

UCLASS(Abstract)
class LASTBELL_API ABoxerCharacter : public ACharacter, public IBoxerInterface
{
    GENERATED_BODY()

public:
    ABoxerCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxerStatsComponent> StatsComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxerFeedbackComponent> FeedbackComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UComboComponent> ComboComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxerAudioComponent> AudioComponent;

    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void LoadFighterData(UFighterDataAsset* Data);

    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void SetBoxerEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Boxer")
    UFighterDataAsset* GetFighterData() const { return FighterData; }

    UFUNCTION(BlueprintPure, Category = "Boxer")
    EBoxerState GetCurrentState() const { return CurrentState; }

    // IBoxerInterface
    virtual void ExecuteAttack_Implementation(EBoxingMove Move) override;
    virtual void ReceiveHit_Implementation(FAttackData AttackData, AActor* Attacker) override;
    virtual void ExecuteDodge_Implementation() override;
    virtual void ExecuteDuck_Implementation() override;
    virtual void StopDuck_Implementation() override;
    virtual EBoxerState GetBoxerState_Implementation() const override;
    virtual float GetHealthPercent_Implementation() const override;
    virtual float GetStaminaPercent_Implementation() const override;
    virtual float GetKOMeterPercent_Implementation() const override;
    virtual bool IsAttacking_Implementation() const override;
    virtual bool IsDucking_Implementation() const override;
    virtual void SetOpponent_Implementation(AActor* InOpponent) override;
    virtual AActor* GetOpponent_Implementation() const override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    EBoxerState CurrentState = EBoxerState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Boxer")
    TObjectPtr<UFighterDataAsset> FighterData;

    TWeakObjectPtr<AActor> OpponentRef;

    void UpdateFacingDirection(float DeltaTime);
    void ClampToRingBounds();

    UFUNCTION()
    void OnKnockdown();

    UFUNCTION()
    void OnKO();

    UFUNCTION()
    void OnHitLanded(AActor* Target, FAttackData AttackData);

    UFUNCTION()
    void OnHitRegistered(EBoxingMove Move);

    UFUNCTION()
    void OnMissRegistered(EBoxingMove Move);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float RingBoundsY = 400.f;

private:
    FTimerHandle HitStunHandle;
    FTimerHandle DodgeHandle;
};
