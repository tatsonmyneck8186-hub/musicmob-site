#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BoxingTypes.h"
#include "BoxerAudioComponent.generated.h"

UCLASS(ClassGroup=(LastBell), meta=(BlueprintSpawnableComponent))
class LASTBELL_API UBoxerAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoxerAudioComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Jab")
    TObjectPtr<USoundBase> JabHeadSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Jab")
    TObjectPtr<USoundBase> JabBodySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Hook")
    TObjectPtr<USoundBase> HookHeadSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Hook")
    TObjectPtr<USoundBase> HookBodySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Uppercut")
    TObjectPtr<USoundBase> UppercutHeadSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Layers")
    TObjectPtr<USoundBase> ImpactFleshLayerSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Layers")
    TObjectPtr<USoundBase> HeavyImpactLayerSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Misc")
    TObjectPtr<USoundBase> SwishMissSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Breathing")
    TObjectPtr<USoundBase> HeavyBreathingLoop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Reactions")
    TObjectPtr<USoundBase> HitReactionGruntSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Reactions")
    TObjectPtr<USoundBase> StaggerSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Reactions")
    TObjectPtr<USoundBase> VictorySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds|Reactions")
    TObjectPtr<USoundBase> DefeatSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    float StaminaBreathingThreshold = 0.35f;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayPunchImpact(EBoxingMove Move, EHitZone Zone, float DamageFraction);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayMissSwish();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayHitReaction();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayStagger();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayVictory();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayDefeat();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void UpdateBreathing(float StaminaPercent);

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float TimeSinceLastImpact = 0.f;
    static constexpr float MinImpactInterval = 0.04f;

    TObjectPtr<UAudioComponent> BreathingAudioComponent;

    void PlaySound(USoundBase* Sound, float VolumeScale = 1.f);
};
