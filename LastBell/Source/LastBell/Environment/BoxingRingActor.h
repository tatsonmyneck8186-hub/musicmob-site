#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxingRingActor.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;

UCLASS()
class LASTBELL_API ABoxingRingActor : public AActor
{
    GENERATED_BODY()

public:
    ABoxingRingActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> FloorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> RopeNorth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> RopeSouth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> RopeEast;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> RopeWest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float RingSize = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float SpotLightIntensity = 8000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    FLinearColor SpotLightColor = FLinearColor(1.f, 0.95f, 0.85f);

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    TArray<TObjectPtr<USpotLightComponent>> CornerLights;

    void BuildRing();
    void BuildLighting();
};
