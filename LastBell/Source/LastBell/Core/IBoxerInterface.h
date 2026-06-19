#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/BoxingTypes.h"
#include "IBoxerInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBoxerInterface : public UInterface
{
    GENERATED_BODY()
};

class LASTBELL_API IBoxerInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void ExecuteAttack(EBoxingMove Move);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void ReceiveHit(FAttackData AttackData, AActor* Attacker);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void ExecuteDodge();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void ExecuteDuck();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void StopDuck();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    EBoxerState GetBoxerState() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    float GetStaminaPercent() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    float GetKOMeterPercent() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    bool IsAttacking() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    bool IsDucking() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    void SetOpponent(AActor* Opponent);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boxing")
    AActor* GetOpponent() const;
};
