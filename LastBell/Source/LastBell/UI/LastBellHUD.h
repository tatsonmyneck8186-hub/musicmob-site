#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LastBellHUD.generated.h"

class UUserWidget;

UCLASS()
class LASTBELL_API ALastBellHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> CharSelectWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> WinScreenWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> LoseScreenWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> KnockdownCountWidgetClass;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowGameHUD();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideGameHUD();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowKnockdownCounter(int32 Count);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayerHealth(float Percent);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateOpponentHealth(float Percent);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayerStamina(float Percent);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateTimer(float RemainingTime);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateCombo(int32 ComboCount);

    /** Draws health/stamina/timer/round/combo with the canvas when no UMG HUD
     *  widget is assigned — lets a code-only boot show a real fight UI. */
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    bool bUseDebugHUD = true;

    virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

private:
    TObjectPtr<UUserWidget> ActiveHUDWidget;
    TObjectPtr<UUserWidget> KnockdownWidget;

    void DrawBar(float X, float Y, float Width, float Height, float Fraction,
        const FLinearColor& FillColor, bool bRightToLeft);
};
