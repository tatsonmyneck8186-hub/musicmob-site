#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BoxingTypes.h"
#include "BoxerStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, NewStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKOMeterChanged, float, NewKOMeter, float, MaxKOMeter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKnockdown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKO);

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UBoxerStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoxerStatsComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStaminaChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnKOMeterChanged OnKOMeterChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnKnockdown OnKnockdown;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnKO OnKO;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void InitializeStats(const FFighterStats& InStats);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyDamage(float RawDamage);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool ConsumeStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void AddKOMeter(float Amount);

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetStaminaPercent() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetKOMeterPercent() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const { return CurrentHealth <= 0.f; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ResetForNewRound();

    float CurrentHealth;
    float MaxHealth;
    float CurrentStamina;
    float MaxStamina;
    float CurrentKOMeter;
    float MaxKOMeter;
    float DefenseMultiplier;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float StaminaRegenRate;
    float StaminaRegenDelay;
    float KOMeterDecayRate;
    float TimeSinceLastStaminaUse;

    void RegenStamina(float DeltaTime);
    void DecayKOMeter(float DeltaTime);
};
