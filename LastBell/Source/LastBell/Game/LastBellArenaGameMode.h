#pragma once

#include "CoreMinimal.h"
#include "Game/LastBellGameMode.h"
#include "Core/BoxingTypes.h"
#include "LastBellArenaGameMode.generated.h"

class AAIBoxer;
class APlayerBoxer;
class UFighterDataAsset;

/** Code-only front-end screens layered over the arena. */
enum class EArenaScreen : uint8
{
    MainMenu,
    OpponentSelect,
    Fighting,
    Result
};

/**
 * Self-contained boot game mode + code-only front end. Drop this into any empty
 * level (it is the project's GlobalDefaultGameMode) and pressing Play opens a
 * main menu -> opponent select -> fight -> win/lose -> rematch/menu loop, all
 * drawn by the HUD canvas with no UMG, Niagara, or authored assets.
 */
UCLASS()
class LASTBELL_API ALastBellArenaGameMode : public ALastBellGameMode
{
    GENERATED_BODY()

public:
    ALastBellArenaGameMode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
    bool bSpawnEnvironment = true;

    EArenaScreen GetScreen() const { return Screen; }
    int32 GetMenuIndex() const { return MenuIndex; }
    int32 GetSelectIndex() const { return SelectIndex; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void HandleMatchEnd(bool bPlayerWon) override;

private:
    EArenaScreen Screen = EArenaScreen::MainMenu;
    int32 MenuIndex = 0;     // 0 = START, 1 = QUIT
    int32 SelectIndex = 0;   // 0 = Rookie, 1 = Counter, 2 = Heavyweight

    UPROPERTY()
    TObjectPtr<APlayerBoxer> PlayerRef;

    UPROPERTY()
    TObjectPtr<AAIBoxer> AIBoxer;

    UPROPERTY()
    TObjectPtr<UFighterDataAsset> CurrentOpponentData;

    // Screen flow.
    void HandleMenuInput();
    void EnterMainMenu();
    void EnterOpponentSelect();
    void StartSelectedFight();
    void ReturnToMenu();

    // World setup.
    void SpawnEnvironment();
    void SpawnLighting();
    AAIBoxer* SpawnAIBoxer(const FVector& Location);

    static EFighterType TypeForIndex(int32 Index);
};
