#include "Components/BoxerFeedbackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformTime.h"

UBoxerFeedbackComponent::UBoxerFeedbackComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBoxerFeedbackComponent::SetFOVOffset(float Delta)
{
    RequestedFOVOffset = Delta;
    bFOVRequested = true;
}

void UBoxerFeedbackComponent::ClearFOVOffset()
{
    RequestedFOVOffset = 0.f;
    bFOVRequested = false;
}

void UBoxerFeedbackComponent::TriggerScreenFlash(float Intensity, float Duration)
{
    OnScreenFlash.Broadcast(Intensity, Duration);
}

void UBoxerFeedbackComponent::TriggerKOSlowMotion(float TargetDilation, float RampInDuration,
    float HoldDuration, float RampOutDuration)
{
    bInSlowMo = true;
    SlowMoTargetDilation = TargetDilation;
    SlowMoRampIn = RampInDuration;
    SlowMoHold = HoldDuration;
    SlowMoRampOut = RampOutDuration;
    SlowMoStartRealTime = FPlatformTime::Seconds();
}

void UBoxerFeedbackComponent::CancelSlowMotion()
{
    bInSlowMo = false;
    UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}

void UBoxerFeedbackComponent::TriggerRumble(float Intensity, float Duration)
{
    // Controller rumble: wire up a UHapticFeedbackEffect_Curve asset in Blueprint
    // and override this function to call PlayHapticEffect with the actual asset.
}

void UBoxerFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bFOVRequested)
    {
        CurrentFOVOffset = FMath::FInterpTo(CurrentFOVOffset, RequestedFOVOffset, DeltaTime, 12.f);
        bFOVRequested = false;
    }
    else
    {
        CurrentFOVOffset = FMath::FInterpTo(CurrentFOVOffset, 0.f, DeltaTime, 8.f);
    }

    if (bInSlowMo)
    {
        TickSlowMo();
    }
}

void UBoxerFeedbackComponent::TickSlowMo()
{
    double Now = FPlatformTime::Seconds();
    double Elapsed = Now - SlowMoStartRealTime;
    double TotalDuration = SlowMoRampIn + SlowMoHold + SlowMoRampOut;

    float TargetDilation = 1.f;

    if (Elapsed < SlowMoRampIn)
    {
        float Alpha = (float)(Elapsed / SlowMoRampIn);
        TargetDilation = FMath::Lerp(1.f, SlowMoTargetDilation, Alpha);
    }
    else if (Elapsed < SlowMoRampIn + SlowMoHold)
    {
        TargetDilation = SlowMoTargetDilation;
    }
    else if (Elapsed < TotalDuration)
    {
        float Alpha = (float)((Elapsed - SlowMoRampIn - SlowMoHold) / SlowMoRampOut);
        TargetDilation = FMath::Lerp(SlowMoTargetDilation, 1.f, Alpha);
    }
    else
    {
        bInSlowMo = false;
        TargetDilation = 1.f;
    }

    UGameplayStatics::SetGlobalTimeDilation(this, TargetDilation);
}
