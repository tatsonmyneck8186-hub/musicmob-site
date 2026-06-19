#include "Environment/CrowdActor.h"
#include "Components/InstancedStaticMeshComponent.h"

ACrowdActor::ACrowdActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.1f;

    CrowdMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CrowdMesh"));
    RootComponent = CrowdMesh;
}

void ACrowdActor::BeginPlay()
{
    Super::BeginPlay();
    SpawnCrowd();
}

void ACrowdActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bCheerLooping)
    {
        CheerTimer += DeltaTime;
        if (CheerTimer >= CheerInterval)
        {
            CheerTimer = 0.f;
            AnimateCheering();
        }
    }
}

void ACrowdActor::SpawnCrowd()
{
    CrowdMesh->ClearInstances();

    for (int32 i = 0; i < CrowdCount; i++)
    {
        float Angle = (float)i / (float)CrowdCount * 2.f * PI;
        float X = FMath::Cos(Angle) * ArenaRadius;
        float Y = FMath::Sin(Angle) * ArenaRadius;
        float Z = 0.f;

        FVector Pos(X, Y, Z);
        FRotator Rot(0.f, FMath::RadiansToDegrees(-Angle) + 90.f, 0.f);
        float Scale = FMath::FRandRange(0.9f, 1.1f);

        FTransform T(Rot, Pos, FVector(Scale));
        CrowdMesh->AddInstance(T);
    }
}

void ACrowdActor::AnimateCheering()
{
    int32 NumInstances = CrowdMesh->GetInstanceCount();
    for (int32 i = 0; i < NumInstances; i++)
    {
        FTransform T;
        CrowdMesh->GetInstanceTransform(i, T, false);
        float ScaleZ = FMath::FRandRange(0.9f, 1.2f);
        T.SetScale3D(FVector(T.GetScale3D().X, T.GetScale3D().Y, ScaleZ));
        CrowdMesh->UpdateInstanceTransform(i, T, false, i == NumInstances - 1);
    }
}

void ACrowdActor::ReactToHeavyHit()
{
    CheerInterval = 0.15f;
    StartCheerLoop();

    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, [this]()
    {
        CheerInterval = 0.5f;
    }, 2.f, false);
}

void ACrowdActor::StartCheerLoop()
{
    bCheerLooping = true;
    CheerTimer = 0.f;
}

void ACrowdActor::QuietDown()
{
    bCheerLooping = false;
}
