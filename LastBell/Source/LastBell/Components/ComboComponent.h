#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BoxingTypes.h"
#include "ComboComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboUpdated, int32, CurrentCombo, int32, BestCombo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UComboComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UComboComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnComboUpdated OnComboUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnComboReset OnComboReset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    float ComboWindowDuration = 2.2f;

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void RegisterHit(EBoxingMove Move);

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void BreakCombo();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void ResetForNewMatch();

    UFUNCTION(BlueprintPure, Category = "Combo")
    int32 GetCurrentCombo() const { return CurrentCombo; }

    UFUNCTION(BlueprintPure, Category = "Combo")
    int32 GetBestCombo() const { return BestCombo; }

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    int32 CurrentCombo = 0;
    int32 BestCombo = 0;
    float TimeSinceLastHit = 0.f;
};
