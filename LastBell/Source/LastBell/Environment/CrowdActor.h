#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdActor.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class LASTBELL_API ACrowdActor : public AActor
{
    GENERATED_BODY()

public:
    ACrowdActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UInstancedStaticMeshComponent> CrowdMesh;

    /** Spectators per stadium tier. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    int32 PerTier = 64;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    int32 TierCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    float BaseRadius = 1050.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    float TierRadiusStep = 260.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    float TierHeightStep = 110.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    FLinearColor CrowdColor = FLinearColor(0.03f, 0.03f, 0.05f);

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void ReactToHeavyHit();

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void StartCheerLoop();

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void QuietDown();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    TArray<FVector> BaseLocations;
    TArray<FRotator> BaseRotations;
    TArray<float> BaseScaleXY;
    TArray<float> Phases;

    float BounceTime = 0.f;
    float BounceAmplitude = 7.f;   // ambient
    float ExcitedTimer = 0.f;       // extra bounce after big hits

    void SpawnCrowd();
};
