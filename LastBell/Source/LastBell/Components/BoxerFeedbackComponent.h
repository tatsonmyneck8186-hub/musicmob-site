#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoxerFeedbackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenFlash, float, Intensity, float, Duration);

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UBoxerFeedbackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoxerFeedbackComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScreenFlash OnScreenFlash;

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void SetFOVOffset(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void ClearFOVOffset();

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    float GetFOVOffset() const { return CurrentFOVOffset; }

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void TriggerScreenFlash(float Intensity, float Duration);

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void TriggerKOSlowMotion(float TargetDilation = 0.1f, float RampInDuration = 0.25f,
        float HoldDuration = 0.6f, float RampOutDuration = 0.4f);

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void TriggerRumble(float Intensity, float Duration);

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    void CancelSlowMotion();

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float CurrentFOVOffset = 0.f;
    float RequestedFOVOffset = 0.f;
    bool bFOVRequested = false;

    bool bInSlowMo = false;
    float SlowMoTargetDilation = 0.1f;
    float SlowMoRampIn = 0.25f;
    float SlowMoHold = 0.6f;
    float SlowMoRampOut = 0.4f;
    double SlowMoStartRealTime = 0.0;

    void TickSlowMo();
};
