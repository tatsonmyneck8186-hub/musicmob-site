#include "Environment/BoxingRingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"

ABoxingRingActor::ABoxingRingActor()
{
    PrimaryActorTick.bCanEverTick = false;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    RootComponent = FloorMesh;

    RopeNorth = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RopeNorth"));
    RopeNorth->SetupAttachment(FloorMesh);

    RopeSouth = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RopeSouth"));
    RopeSouth->SetupAttachment(FloorMesh);

    RopeEast = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RopeEast"));
    RopeEast->SetupAttachment(FloorMesh);

    RopeWest = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RopeWest"));
    RopeWest->SetupAttachment(FloorMesh);
}

void ABoxingRingActor::BeginPlay()
{
    Super::BeginPlay();
    BuildRing();
    BuildLighting();
}

void ABoxingRingActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}

void ABoxingRingActor::BuildRing()
{
    float Half = RingSize * 0.5f;

    FloorMesh->SetRelativeScale3D(FVector(RingSize / 100.f, RingSize / 100.f, 0.2f));

    RopeNorth->SetRelativeLocation(FVector(Half, 0.f, 50.f));
    RopeNorth->SetRelativeScale3D(FVector(0.2f, RingSize / 100.f, 0.5f));

    RopeSouth->SetRelativeLocation(FVector(-Half, 0.f, 50.f));
    RopeSouth->SetRelativeScale3D(FVector(0.2f, RingSize / 100.f, 0.5f));

    RopeEast->SetRelativeLocation(FVector(0.f, Half, 50.f));
    RopeEast->SetRelativeScale3D(FVector(RingSize / 100.f, 0.2f, 0.5f));

    RopeWest->SetRelativeLocation(FVector(0.f, -Half, 50.f));
    RopeWest->SetRelativeScale3D(FVector(RingSize / 100.f, 0.2f, 0.5f));
}

void ABoxingRingActor::BuildLighting()
{
    TArray<FVector2D> Corners = {
        FVector2D( 1.f,  1.f),
        FVector2D( 1.f, -1.f),
        FVector2D(-1.f,  1.f),
        FVector2D(-1.f, -1.f)
    };

    float Half = RingSize * 0.5f;

    auto ConfigSpot = [&](USpotLightComponent* Light, float LX, float LY, float Yaw)
    {
        Light->SetRelativeLocation(FVector(LX, LY, 700.f));
        Light->SetRelativeRotation(FRotator(-80.f, Yaw, 0.f));
        Light->SetIntensity(SpotLightIntensity);
        Light->SetLightColor(SpotLightColor);
        Light->InnerConeAngle = 15.f;
        Light->OuterConeAngle = 35.f;
    };

    for (int32 i = 0; i < 4; i++)
    {
        USpotLightComponent* Light = NewObject<USpotLightComponent>(this,
            *FString::Printf(TEXT("SpotLight_%d"), i));
        Light->RegisterComponent();
        Light->AttachToComponent(FloorMesh, FAttachmentTransformRules::KeepRelativeTransform);
        CornerLights.Add(Light);

        float LX = Corners[i].X * Half;
        float LY = Corners[i].Y * Half;
        float Yaw = FMath::RadiansToDegrees(FMath::Atan2(-LY, -LX));
        ConfigSpot(Light, LX, LY, Yaw);
    }
}
