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
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInstanceDynamic;

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

    // ---- Procedural humanoid rig (placeholder fighter built from basic shapes) ----
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<USceneComponent> BodyRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> Pelvis;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> Torso;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> Head;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> LeftShoulder;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> RightShoulder;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> LeftGlove;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> RightGlove;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> LeftLeg;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
    TObjectPtr<UStaticMeshComponent> RightLeg;

    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void LoadFighterData(UFighterDataAsset* Data);

    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void SetBoxerEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Boxer")
    UFighterDataAsset* GetFighterData() const { return FighterData; }

    UFUNCTION(BlueprintPure, Category = "Boxer")
    EBoxerState GetCurrentState() const { return CurrentState; }

    /** Set the visual victory/defeat pose when a match ends. */
    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void SetEndPose(bool bWon);

    /** Clear transient combat/animation state back to a neutral guard. */
    UFUNCTION(BlueprintCallable, Category = "Boxer")
    void ResetState();

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

    UFUNCTION()
    void OnAttackPhaseChanged(EAttackPhase NewPhase);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float RingBoundsY = 400.f;

private:
    FTimerHandle HitStunHandle;
    FTimerHandle DodgeHandle;

    // ---- Procedural body construction + animation ----
    void BuildBody();
    void TintBody();
    void UpdateProceduralAnim(float DeltaTime);
    UStaticMeshComponent* MakePart(const FName& Name, USceneComponent* Parent);

    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> SkinMat;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> TrunkMat;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> BodyMat;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> GloveMat;

    float AnimTime = 0.f;
    float DodgeDir = 1.f;          // +1 right, -1 left
    bool bEndPoseWon = false;
    bool bInEndPose = false;
    EBoxingMove LastMove = EBoxingMove::None;
};
