#include "Environment/CrowdActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

ACrowdActor::ACrowdActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.05f;

    CrowdMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CrowdMesh"));
    RootComponent = CrowdMesh;
    CrowdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CrowdMesh->SetCastShadow(false);
}

void ACrowdActor::BeginPlay()
{
    Super::BeginPlay();

    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        CrowdMesh->SetStaticMesh(Cube);
    }
    if (UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
    {
        if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Mat, this))
        {
            MID->SetVectorParameterValue(TEXT("Color"), CrowdColor);
            CrowdMesh->SetMaterial(0, MID);
        }
    }

    SpawnCrowd();
}

void ACrowdActor::SpawnCrowd()
{
    CrowdMesh->ClearInstances();
    BaseLocations.Reset();
    BaseRotations.Reset();
    BaseScaleXY.Reset();
    Phases.Reset();

    for (int32 Tier = 0; Tier < TierCount; ++Tier)
    {
        const float Radius = BaseRadius + Tier * TierRadiusStep;
        const float Height = Tier * TierHeightStep;
        const int32 Count = PerTier + Tier * 12;

        for (int32 i = 0; i < Count; ++i)
        {
            const float Angle = (float)i / (float)Count * 2.f * PI + Tier * 0.15f;
            const float X = FMath::Cos(Angle) * Radius;
            const float Y = FMath::Sin(Angle) * Radius;

            // Spectators are upright slabs (a readable silhouette), facing the ring.
            const float ScaleXY = FMath::FRandRange(0.32f, 0.46f);
            const float ScaleZ = FMath::FRandRange(1.5f, 2.1f);
            const FVector Loc(X, Y, Height + ScaleZ * 50.f);
            const FRotator Rot(0.f, FMath::RadiansToDegrees(-Angle) + 90.f, 0.f);

            BaseLocations.Add(Loc);
            BaseRotations.Add(Rot);
            BaseScaleXY.Add(ScaleXY);
            Phases.Add(FMath::FRandRange(0.f, 2.f * PI));

            CrowdMesh->AddInstance(FTransform(Rot, Loc, FVector(ScaleXY, ScaleXY, ScaleZ)));
        }
    }
}

void ACrowdActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    BounceTime += DeltaTime;
    if (ExcitedTimer > 0.f) ExcitedTimer -= DeltaTime;
    const float Amp = BounceAmplitude + (ExcitedTimer > 0.f ? 22.f : 0.f);
    const float Speed = (ExcitedTimer > 0.f) ? 9.f : 4.f;

    const int32 Num = CrowdMesh->GetInstanceCount();
    for (int32 i = 0; i < Num && i < BaseLocations.Num(); ++i)
    {
        const float Bounce = FMath::Max(0.f, FMath::Sin(BounceTime * Speed + Phases[i])) * Amp;
        FVector Loc = BaseLocations[i];
        Loc.Z += Bounce;

        FTransform T;
        CrowdMesh->GetInstanceTransform(i, T, false);
        const FVector S = T.GetScale3D();
        CrowdMesh->UpdateInstanceTransform(i, FTransform(BaseRotations[i], Loc, S), false, i == Num - 1, false);
    }
}

void ACrowdActor::ReactToHeavyHit()
{
    ExcitedTimer = 1.8f;
}

void ACrowdActor::StartCheerLoop()
{
    ExcitedTimer = FMath::Max(ExcitedTimer, 1.0f);
}

void ACrowdActor::QuietDown()
{
    ExcitedTimer = 0.f;
}
