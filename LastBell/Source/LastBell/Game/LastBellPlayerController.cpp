#include "Game/LastBellPlayerController.h"
#include "Kismet/GameplayStatics.h"

ALastBellPlayerController::ALastBellPlayerController()
{
}

void ALastBellPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}

void ALastBellPlayerController::ShowWinScreen()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}

void ALastBellPlayerController::ShowLoseScreen()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}

void ALastBellPlayerController::ReturnToMainMenu()
{
    UGameplayStatics::OpenLevel(this, FName("L_MainMenu"));
}
