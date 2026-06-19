#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BoxingTypes.h"
#include "BoxingAIPersonalityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAggressionStateChanged, EAggressionState, NewState);

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UBoxingAIPersonalityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoxingAIPersonalityComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAggressionStateChanged OnAggressionStateChanged;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void Initialize(const FAIPersonality& InPersonality);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnPunchLanded();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnPunchReceived(float DamageFraction);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnHealthUpdated(float HealthPercent);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void RecordFakePunch();

    UFUNCTION(BlueprintPure, Category = "AI")
    bool CanFakePunch() const;

    UFUNCTION(BlueprintPure, Category = "AI")
    EAggressionState GetAggressionState() const { return CurrentAggressionState; }

    UFUNCTION(BlueprintPure, Category = "AI")
    float GetFatigueLevel() const { return FatigueLevel; }

    UFUNCTION(BlueprintPure, Category = "AI")
    float GetAttackFrequencyMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsInComebackMode() const { return bComebackMode; }

    UFUNCTION(BlueprintPure, Category = "AI")
    float GetDodgeProbability() const;

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FAIPersonality Personality;
    EAggressionState CurrentAggressionState = EAggressionState::Normal;
    float FatigueLevel = 0.f;
    float TimeSinceFakePunch = 999.f;
    bool bComebackMode = false;
    float CurrentHealthPercent = 1.f;

    void UpdateAggressionState();
    void TickFatigue(float DeltaTime);
};
