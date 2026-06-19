#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoxingUserWidget.generated.h"

class ALastBellGameMode;
class ULastBellGameInstance;

UCLASS()
class LASTBELL_API UBoxingUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Game")
    ALastBellGameMode* GetLastBellGameMode() const;

    UFUNCTION(BlueprintPure, Category = "Game")
    ULastBellGameInstance* GetLastBellGameInstance() const;

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void GoToMainMenu();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void GoToCharacterSelect();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void StartFight();

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void QuitGame();
};
