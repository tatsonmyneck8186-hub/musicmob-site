#pragma once

#include "CoreMinimal.h"
#include "Characters/BoxerCharacter.h"
#include "InputActionValue.h"
#include "PlayerBoxer.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UCameraShakeBase;

UCLASS()
class LASTBELL_API APlayerBoxer : public ABoxerCharacter
{
    GENERATED_BODY()

public:
    APlayerBoxer();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_MoveHorizontal;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Jab;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Hook;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Uppercut;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Dodge;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Duck;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float BaseFOV = 78.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    TSubclassOf<UCameraShakeBase> ShakeLight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    TSubclassOf<UCameraShakeBase> ShakeHeavy;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    TSubclassOf<UCameraShakeBase> ShakeKO;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void ReceiveHit_Implementation(FAttackData AttackData, AActor* Attacker) override;

    UFUNCTION()
    void OnPlayerLandedHit(AActor* Target, FAttackData AttackData);

private:
    float HorizontalInput = 0.f;

    void UpdateArcadeCamera(float DeltaTime);
    void PlayImpactShake(bool bHeavy);

    float ImpactZoom = 0.f;   // transient FOV pulse on impact (decays to 0)
    float SwayTime = 0.f;

    /** Builds Enhanced Input assets in C++ when none are assigned, so the game is
     *  playable from code alone (A/D move, LMB/RMB/E punch, Space dodge, S duck). */
    void EnsureRuntimeInput();

    void OnMoveHorizontal(const FInputActionValue& Value);
    void OnJab(const FInputActionValue& Value);
    void OnHook(const FInputActionValue& Value);
    void OnUppercut(const FInputActionValue& Value);
    void OnDodge(const FInputActionValue& Value);
    void OnDuckPressed(const FInputActionValue& Value);
    void OnDuckReleased(const FInputActionValue& Value);
};
