#include "UI/LastBellHUD.h"
#include "Blueprint/UserWidget.h"

void ALastBellHUD::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBellHUD::ShowGameHUD()
{
    if (HUDWidgetClass && !ActiveHUDWidget)
    {
        ActiveHUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HUDWidgetClass);
        if (ActiveHUDWidget)
        {
            ActiveHUDWidget->AddToViewport();
        }
    }
}

void ALastBellHUD::HideGameHUD()
{
    if (ActiveHUDWidget)
    {
        ActiveHUDWidget->RemoveFromParent();
        ActiveHUDWidget = nullptr;
    }
}

void ALastBellHUD::ShowKnockdownCounter(int32 Count)
{
    if (!KnockdownWidget && KnockdownCountWidgetClass)
    {
        KnockdownWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), KnockdownCountWidgetClass);
        if (KnockdownWidget)
        {
            KnockdownWidget->AddToViewport(10);
        }
    }
}
