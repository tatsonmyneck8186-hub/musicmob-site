#include "UI/LastBellHUD.h"
#include "Blueprint/UserWidget.h"
#include "Game/LastBellGameMode.h"
#include "Characters/BoxerCharacter.h"
#include "Components/ComboComponent.h"
#include "Data/FighterDataAsset.h"
#include "Core/IBoxerInterface.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void ALastBellHUD::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBellHUD::DrawBar(float X, float Y, float Width, float Height, float Fraction,
    const FLinearColor& FillColor, bool bRightToLeft)
{
    Fraction = FMath::Clamp(Fraction, 0.f, 1.f);
    // Backing plate.
    DrawRect(FLinearColor(0.02f, 0.02f, 0.02f, 0.75f), X - 2.f, Y - 2.f, Width + 4.f, Height + 4.f);
    const float FillW = Width * Fraction;
    const float FillX = bRightToLeft ? (X + Width - FillW) : X;
    DrawRect(FillColor, FillX, Y, FillW, Height);
}

void ALastBellHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!bUseDebugHUD || ActiveHUDWidget || !Canvas)
    {
        return;
    }

    ALastBellGameMode* GM = Cast<ALastBellGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GM)
    {
        return;
    }

    const float ScreenW = Canvas->SizeX;
    const float Margin = 40.f;
    const float BarW = ScreenW * 0.34f;
    const float BarH = 22.f;

    AActor* Player = Cast<AActor>(GM->PlayerBoxer);
    AActor* Opponent = Cast<AActor>(GM->AIBoxerRef);

    if (Player && Player->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        const float HP = IBoxerInterface::Execute_GetHealthPercent(Player);
        const float ST = IBoxerInterface::Execute_GetStaminaPercent(Player);
        DrawBar(Margin, 36.f, BarW, BarH, HP, FLinearColor(0.85f, 0.15f, 0.15f), false);
        DrawBar(Margin, 36.f + BarH + 6.f, BarW, 10.f, ST, FLinearColor(0.95f, 0.8f, 0.1f), false);
        DrawText(TEXT("YOU"), FLinearColor::White, Margin, 14.f, nullptr, 1.1f);
    }

    if (Opponent && Opponent->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        const float HP = IBoxerInterface::Execute_GetHealthPercent(Opponent);
        const float ST = IBoxerInterface::Execute_GetStaminaPercent(Opponent);
        const float RX = ScreenW - Margin - BarW;
        DrawBar(RX, 36.f, BarW, BarH, HP, FLinearColor(0.85f, 0.15f, 0.15f), true);
        DrawBar(RX, 36.f + BarH + 6.f, BarW, 10.f, ST, FLinearColor(0.95f, 0.8f, 0.1f), true);

        FString OppName = TEXT("OPPONENT");
        if (ABoxerCharacter* OppChar = Cast<ABoxerCharacter>(Opponent))
        {
            if (OppChar->GetFighterData())
            {
                OppName = OppChar->GetFighterData()->FighterName.ToString();
            }
        }
        DrawText(OppName, FLinearColor::White, RX, 14.f, nullptr, 1.1f);
    }

    // Round timer + round number, centered.
    const int32 TimeLeft = FMath::Max(0, FMath::CeilToInt(GM->GetRoundTimeRemaining()));
    const FString TimerStr = FString::Printf(TEXT("%02d"), TimeLeft);
    DrawText(TimerStr, FLinearColor::White, ScreenW * 0.5f - 18.f, 24.f, nullptr, 2.0f);
    DrawText(FString::Printf(TEXT("ROUND %d"), FMath::Max(1, GM->GetCurrentRound())),
        FLinearColor(0.9f, 0.9f, 0.9f), ScreenW * 0.5f - 36.f, 70.f, nullptr, 1.0f);

    // Player combo counter.
    if (ABoxerCharacter* PChar = Cast<ABoxerCharacter>(Player))
    {
        if (PChar->ComboComponent)
        {
            const int32 Combo = PChar->ComboComponent->GetCurrentCombo();
            if (Combo >= 2)
            {
                DrawText(FString::Printf(TEXT("%d HIT COMBO"), Combo),
                    FLinearColor(1.f, 0.6f, 0.1f), Margin, 90.f, nullptr, 1.4f);
            }
        }
    }
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
