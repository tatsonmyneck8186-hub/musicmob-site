#include "Components/BoxerAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

UBoxerAudioComponent::UBoxerAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}

void UBoxerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TimeSinceLastImpact += DeltaTime;
}

void UBoxerAudioComponent::PlayPunchImpact(EBoxingMove Move, EHitZone Zone, float DamageFraction)
{
    if (TimeSinceLastImpact < MinImpactInterval) return;
    TimeSinceLastImpact = 0.f;

    USoundBase* Primary = nullptr;
    switch (Move)
    {
    case EBoxingMove::Jab:
        Primary = (Zone == EHitZone::Head) ? JabHeadSound : JabBodySound;
        break;
    case EBoxingMove::Hook:
        Primary = (Zone == EHitZone::Head) ? HookHeadSound : HookBodySound;
        break;
    case EBoxingMove::Uppercut:
        Primary = UppercutHeadSound;
        break;
    default:
        break;
    }

    PlaySound(Primary);
    PlaySound(ImpactFleshLayerSound, 0.6f);

    if (DamageFraction > 0.6f || Move == EBoxingMove::Uppercut)
    {
        PlaySound(HeavyImpactLayerSound, DamageFraction);
    }
}

void UBoxerAudioComponent::PlayMissSwish()
{
    PlaySound(SwishMissSound, 0.5f);
}

void UBoxerAudioComponent::PlayHitReaction()
{
    PlaySound(HitReactionGruntSound);
}

void UBoxerAudioComponent::PlayStagger()
{
    PlaySound(StaggerSound);
}

void UBoxerAudioComponent::PlayVictory()
{
    PlaySound(VictorySound);
}

void UBoxerAudioComponent::PlayDefeat()
{
    PlaySound(DefeatSound);
}

void UBoxerAudioComponent::UpdateBreathing(float StaminaPercent)
{
    if (!HeavyBreathingLoop) return;

    if (StaminaPercent < StaminaBreathingThreshold)
    {
        if (!BreathingAudioComponent)
        {
            BreathingAudioComponent = UGameplayStatics::SpawnSoundAttached(
                HeavyBreathingLoop, Cast<USceneComponent>(GetOwner()->GetRootComponent()));
        }
        if (BreathingAudioComponent)
        {
            float Volume = FMath::GetMappedRangeValueClamped(
                FVector2D(0.f, StaminaBreathingThreshold),
                FVector2D(1.f, 0.f),
                StaminaPercent);
            BreathingAudioComponent->SetVolumeMultiplier(Volume);
        }
    }
    else if (BreathingAudioComponent)
    {
        BreathingAudioComponent->FadeOut(0.5f, 0.f);
        BreathingAudioComponent = nullptr;
    }
}

void UBoxerAudioComponent::PlaySound(USoundBase* Sound, float VolumeScale)
{
    if (!Sound) return;
    UGameplayStatics::PlaySoundAtLocation(this, Sound, GetOwner()->GetActorLocation(), VolumeScale);
}
