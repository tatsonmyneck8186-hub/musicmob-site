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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    int32 CrowdCount = 120;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
    float ArenaRadius = 1200.f;

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void AnimateCheering();

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
    bool bCheerLooping = false;
    float CheerTimer = 0.f;
    float CheerInterval = 0.5f;

    void SpawnCrowd();
};
