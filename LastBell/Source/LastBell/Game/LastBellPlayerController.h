#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/BoxingTypes.h"
#include "LastBellPlayerController.generated.h"

UCLASS()
class LASTBELL_API ALastBellPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALastBellPlayerController();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowWinScreen();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowLoseScreen();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ReturnToMainMenu();

    UFUNCTION(BlueprintImplementableEvent, Category = "Match")
    void OnMatchStateChanged(EMatchState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Match")
    void OnKnockdownCountChanged(int32 Count);

protected:
    virtual void BeginPlay() override;
};
