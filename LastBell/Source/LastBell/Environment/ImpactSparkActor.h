#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImpactSparkActor.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UMaterialInstanceDynamic;

/**
 * A short-lived procedural punch-impact effect: a bright burst sphere that pops
 * and shrinks plus a point-light flash that fades, then self-destructs. Built
 * entirely from engine primitives (no Niagara). Use Init() right after spawning.
 */
UCLASS()
class LASTBELL_API AImpactSparkActor : public AActor
{
    GENERATED_BODY()

public:
    AImpactSparkActor();

    /** Configure the burst. bBigFlash = a larger, longer KO flash. */
    UFUNCTION(BlueprintCallable, Category = "Impact")
    void Init(const FLinearColor& Color, float ScaleMultiplier, bool bBigFlash);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY() TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Burst;
    UPROPERTY() TObjectPtr<UPointLightComponent> Flash;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> BurstMID;

    FLinearColor BurstColor = FLinearColor(1.f, 0.95f, 0.7f);
    float Age = 0.f;
    float Life = 0.22f;
    float PeakScale = 0.7f;
    float FlashIntensity = 6000.f;
    float FlashRadius = 320.f;

    void ApplyColor();
};
