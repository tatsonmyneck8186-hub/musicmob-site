#include "Environment/ImpactSparkActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AImpactSparkActor::AImpactSparkActor()
{
    PrimaryActorTick.bCanEverTick = true;
    InitialLifeSpan = 1.5f; // hard safety net; we Destroy() ourselves earlier

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    Burst = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Burst"));
    Burst->SetupAttachment(SceneRoot);
    Burst->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Burst->SetCastShadow(false);
    Burst->SetRelativeScale3D(FVector(0.1f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded()) Burst->SetStaticMesh(SphereMesh.Object);
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (ShapeMat.Succeeded()) Burst->SetMaterial(0, ShapeMat.Object);

    Flash = CreateDefaultSubobject<UPointLightComponent>(TEXT("Flash"));
    Flash->SetupAttachment(SceneRoot);
    Flash->SetCastShadows(false);
    Flash->SetIntensity(FlashIntensity);
    Flash->SetAttenuationRadius(FlashRadius);
    Flash->SetLightColor(BurstColor);
}

void AImpactSparkActor::Init(const FLinearColor& Color, float ScaleMultiplier, bool bBigFlash)
{
    BurstColor = Color;
    PeakScale = (bBigFlash ? 2.2f : 0.7f) * FMath::Max(0.2f, ScaleMultiplier);
    Life = bBigFlash ? 0.5f : 0.22f;
    FlashIntensity = bBigFlash ? 22000.f : 7000.f;
    FlashRadius = bBigFlash ? 900.f : 340.f;

    if (Flash)
    {
        Flash->SetIntensity(FlashIntensity);
        Flash->SetAttenuationRadius(FlashRadius);
        Flash->SetLightColor(BurstColor);
    }
    ApplyColor();
}

void AImpactSparkActor::ApplyColor()
{
    if (!BurstMID && Burst)
    {
        BurstMID = Burst->CreateDynamicMaterialInstance(0);
    }
    if (BurstMID)
    {
        BurstMID->SetVectorParameterValue(TEXT("Color"), BurstColor);
    }
}

void AImpactSparkActor::BeginPlay()
{
    Super::BeginPlay();
    ApplyColor();
}

void AImpactSparkActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Age += DeltaTime;
    const float A = FMath::Clamp(Age / Life, 0.f, 1.f);

    // Pop out fast, then shrink away.
    const float ScaleCurve = (A < 0.35f)
        ? FMath::Lerp(0.1f, PeakScale, A / 0.35f)
        : FMath::Lerp(PeakScale, 0.02f, (A - 0.35f) / 0.65f);
    if (Burst) Burst->SetWorldScale3D(FVector(ScaleCurve));

    if (Flash) Flash->SetIntensity(FlashIntensity * (1.f - A));

    if (Age >= Life)
    {
        Destroy();
    }
}
