#include "Environment/BoxingRingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

ABoxingRingActor::ABoxingRingActor()
{
    PrimaryActorTick.bCanEverTick = false;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    RootComponent = FloorMesh;
}

void ABoxingRingActor::BeginPlay()
{
    Super::BeginPlay();

    CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    ShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

    BuildRing();
    BuildLighting();
}

UStaticMeshComponent* ABoxingRingActor::AddPart(UStaticMesh* Mesh, const FVector& Loc, const FVector& Scale,
    const FRotator& Rot, const FLinearColor& Color)
{
    UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(this);
    Comp->RegisterComponent();
    Comp->AttachToComponent(FloorMesh, FAttachmentTransformRules::KeepRelativeTransform);
    if (Mesh) Comp->SetStaticMesh(Mesh);
    if (ShapeMaterial) Comp->SetMaterial(0, ShapeMaterial);
    Comp->SetRelativeLocation(Loc);
    Comp->SetRelativeRotation(Rot);
    Comp->SetRelativeScale3D(Scale);
    Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Comp->SetCastShadow(true);
    if (UMaterialInstanceDynamic* MID = Comp->CreateDynamicMaterialInstance(0))
    {
        MID->SetVectorParameterValue(TEXT("Color"), Color);
    }
    Parts.Add(Comp);
    return Comp;
}

void ABoxingRingActor::BuildRing()
{
    const float Half = RingSize * 0.5f;
    const float ApronHalf = Half + ApronWidth;

    // --- Mat (fighting surface, has collision; this is the root FloorMesh) ---
    if (CubeMesh) FloorMesh->SetStaticMesh(CubeMesh);
    if (ShapeMaterial) FloorMesh->SetMaterial(0, ShapeMaterial);
    FloorMesh->SetRelativeScale3D(FVector(RingSize / 100.f, RingSize / 100.f, 0.2f)); // top ~ +10
    if (UMaterialInstanceDynamic* MatMID = FloorMesh->CreateDynamicMaterialInstance(0))
    {
        MatMID->SetVectorParameterValue(TEXT("Color"), MatColor);
    }

    // --- Center mat logo (thin disc just above the mat) ---
    AddPart(CylinderMesh, FVector(0.f, 0.f, 11.f),
        FVector(RingSize * 0.34f / 100.f, RingSize * 0.34f / 100.f, 0.03f), FRotator::ZeroRotator, LogoColor);

    // --- Apron: canvas ledge ringing the mat, slightly below mat top ---
    AddPart(CubeMesh, FVector(0.f, 0.f, -4.f),
        FVector(ApronHalf * 2.f / 100.f, ApronHalf * 2.f / 100.f, 0.10f), FRotator::ZeroRotator, ApronColor);

    // --- Raised platform body (skirt down to the arena floor) ---
    AddPart(CubeMesh, FVector(0.f, 0.f, -52.f),
        FVector(ApronHalf * 2.f / 100.f, ApronHalf * 2.f / 100.f, 0.9f), FRotator::ZeroRotator,
        FLinearColor(0.04f, 0.04f, 0.05f));

    // --- 4 corner posts + 3-tier ropes + turnbuckle pads ---
    const FVector2D Corners[4] = {
        FVector2D( Half,  Half), FVector2D( Half, -Half),
        FVector2D(-Half,  Half), FVector2D(-Half, -Half)
    };

    const float PostCenterZ = PostHeight * 0.5f + 10.f;
    for (int32 i = 0; i < 4; ++i)
    {
        const FVector PostLoc(Corners[i].X, Corners[i].Y, PostCenterZ);
        AddPart(CylinderMesh, PostLoc, FVector(0.16f, 0.16f, PostHeight / 100.f), FRotator::ZeroRotator, PostColor);

        // Turnbuckle pad near the top of each post (team colors alternate).
        const FLinearColor PadColor = (i % 2 == 0) ? FLinearColor(0.85f, 0.12f, 0.12f) : FLinearColor(0.12f, 0.25f, 0.85f);
        AddPart(CubeMesh, FVector(Corners[i].X * 0.92f, Corners[i].Y * 0.92f, PostHeight * 0.78f),
            FVector(0.30f, 0.30f, 0.55f), FRotator::ZeroRotator, PadColor);
    }

    // Three rope tiers per side.
    const float RopeZ[3] = { 55.f, 100.f, 145.f };
    const float RopeThick = 0.05f;
    for (int32 t = 0; t < 3; ++t)
    {
        const float Z = RopeZ[t];
        // North / South (constant X = +-Half), spanning Y.
        AddPart(CubeMesh, FVector( Half, 0.f, Z), FVector(RopeThick, RingSize / 100.f, RopeThick), FRotator::ZeroRotator, RopeColor);
        AddPart(CubeMesh, FVector(-Half, 0.f, Z), FVector(RopeThick, RingSize / 100.f, RopeThick), FRotator::ZeroRotator, RopeColor);
        // East / West (constant Y = +-Half), spanning X.
        AddPart(CubeMesh, FVector(0.f,  Half, Z), FVector(RingSize / 100.f, RopeThick, RopeThick), FRotator::ZeroRotator, RopeColor);
        AddPart(CubeMesh, FVector(0.f, -Half, Z), FVector(RingSize / 100.f, RopeThick, RopeThick), FRotator::ZeroRotator, RopeColor);
    }
}

void ABoxingRingActor::BuildLighting()
{
    const float Half = RingSize * 0.5f;

    const FVector2D Corners[4] = {
        FVector2D( 1.f,  1.f), FVector2D( 1.f, -1.f),
        FVector2D(-1.f,  1.f), FVector2D(-1.f, -1.f)
    };

    auto ConfigSpot = [&](USpotLightComponent* Light, float LX, float LY, float Yaw)
    {
        Light->SetRelativeLocation(FVector(LX, LY, 720.f));
        Light->SetRelativeRotation(FRotator(-80.f, Yaw, 0.f));
        Light->SetIntensity(SpotLightIntensity);
        Light->SetLightColor(SpotLightColor);
        Light->SetInnerConeAngle(16.f);
        Light->SetOuterConeAngle(38.f);
        Light->SetAttenuationRadius(2000.f);
        Light->SetCastShadows(true);
    };

    for (int32 i = 0; i < 4; ++i)
    {
        USpotLightComponent* Light = NewObject<USpotLightComponent>(this);
        Light->RegisterComponent();
        Light->AttachToComponent(FloorMesh, FAttachmentTransformRules::KeepRelativeTransform);
        CornerLights.Add(Light);

        const float LX = Corners[i].X * Half;
        const float LY = Corners[i].Y * Half;
        const float Yaw = FMath::RadiansToDegrees(FMath::Atan2(-LY, -LX));
        ConfigSpot(Light, LX, LY, Yaw);
    }
}
