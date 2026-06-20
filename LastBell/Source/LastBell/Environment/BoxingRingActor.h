#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxingRingActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USpotLightComponent;
class UStaticMesh;
class UMaterialInterface;

UCLASS()
class LASTBELL_API ABoxingRingActor : public AActor
{
    GENERATED_BODY()

public:
    ABoxingRingActor();

    /** The mat / fighting surface (has collision so fighters stand on it). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> FloorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float RingSize = 860.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float ApronWidth = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
    float PostHeight = 175.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring|Color")
    FLinearColor MatColor = FLinearColor(0.06f, 0.20f, 0.45f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring|Color")
    FLinearColor LogoColor = FLinearColor(0.95f, 0.78f, 0.15f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring|Color")
    FLinearColor ApronColor = FLinearColor(0.5f, 0.05f, 0.08f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring|Color")
    FLinearColor RopeColor = FLinearColor(0.9f, 0.9f, 0.92f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring|Color")
    FLinearColor PostColor = FLinearColor(0.15f, 0.16f, 0.18f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float SpotLightIntensity = 9000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    FLinearColor SpotLightColor = FLinearColor(1.f, 0.95f, 0.85f);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY() TArray<TObjectPtr<USceneComponent>> Parts;
    UPROPERTY() TArray<TObjectPtr<USpotLightComponent>> CornerLights;

    UPROPERTY() TObjectPtr<UStaticMesh> CubeMesh;
    UPROPERTY() TObjectPtr<UStaticMesh> CylinderMesh;
    UPROPERTY() TObjectPtr<UMaterialInterface> ShapeMaterial;

    UStaticMeshComponent* AddPart(UStaticMesh* Mesh, const FVector& Loc, const FVector& Scale,
        const FRotator& Rot, const FLinearColor& Color);

    void BuildRing();
    void BuildLighting();
};
