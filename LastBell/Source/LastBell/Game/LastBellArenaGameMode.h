#pragma once

#include "CoreMinimal.h"
#include "Game/LastBellGameMode.h"
#include "Core/BoxingTypes.h"
#include "LastBellArenaGameMode.generated.h"

class AAIBoxer;

/**
 * A self-contained boot game mode. Drop this into any empty level (it is already
 * the project's GlobalDefaultGameMode) and pressing Play spawns the ring, crowd,
 * lighting, both fighters, runtime fighter data, and starts the match — no
 * hand-authored maps or Data Assets required.
 */
UCLASS()
class LASTBELL_API ALastBellArenaGameMode : public ALastBellGameMode
{
    GENERATED_BODY()

public:
    ALastBellArenaGameMode();

    /** Which archetype the player faces when booting straight into the arena. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
    EFighterType OpponentType = EFighterType::Rookie;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
    bool bSpawnEnvironment = true;

protected:
    virtual void BeginPlay() override;

private:
    void SpawnEnvironment();
    void SpawnLighting();
    AAIBoxer* SpawnAIBoxer(const FVector& Location);
};
