#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/BoxingTypes.h"
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

    /** Draws a bold arcade HUD with the canvas when no UMG HUD widget is assigned. */
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    bool bUseDebugHUD = true;

    virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

private:
    TObjectPtr<UUserWidget> ActiveHUDWidget;
    TObjectPtr<UUserWidget> KnockdownWidget;

    // Canvas HUD transient state.
    float KOFlashTimer = 0.f;       // white impact flash countdown
    float ComboPulse = 0.f;         // combo emphasis pulse
    int32 LastComboShown = 0;
    float LastPlayerHealth = 1.f;
    float LastOppHealth = 1.f;
    float FightFlashTimer = 0.f;    // "FIGHT!" banner at the bell
    EMatchState LastMatchState = EMatchState::WaitingToStart;

    void DrawPanelBar(float X, float Y, float W, float H, float Fraction,
        const FLinearColor& Fill, bool bRightToLeft, const FString& Label);
    void DrawShadowText(const FString& Text, const FLinearColor& Color,
        float X, float Y, float Scale, bool bCenter);

    void DrawFightHUD(class ALastBellGameMode* GM);
    void DrawMainMenu(int32 MenuIndex);
    void DrawOpponentSelect(int32 SelectIndex);
    void DrawResultBanner(bool bPlayerWon);
};
