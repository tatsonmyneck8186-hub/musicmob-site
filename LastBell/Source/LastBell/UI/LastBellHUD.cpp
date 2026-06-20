#include "UI/LastBellHUD.h"
#include "Blueprint/UserWidget.h"
#include "Game/LastBellGameMode.h"
#include "Game/LastBellArenaGameMode.h"
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

void ALastBellHUD::DrawShadowText(const FString& Text, const FLinearColor& Color,
    float X, float Y, float Scale, bool bCenter)
{
    float TW = 0.f, TH = 0.f;
    GetTextSize(Text, TW, TH, nullptr, Scale);
    const float DrawX = bCenter ? X - TW * 0.5f : X;
    DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.85f), DrawX + 2.f, Y + 2.f, nullptr, Scale);
    DrawText(Text, Color, DrawX, Y, nullptr, Scale);
}

void ALastBellHUD::DrawPanelBar(float X, float Y, float W, float H, float Fraction,
    const FLinearColor& Fill, bool bRightToLeft, const FString& Label)
{
    Fraction = FMath::Clamp(Fraction, 0.f, 1.f);
    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.85f), X - 3.f, Y - 3.f, W + 6.f, H + 6.f);
    DrawRect(FLinearColor(0.10f, 0.10f, 0.12f, 1.f), X, Y, W, H);
    const float FillW = W * Fraction;
    const float FillX = bRightToLeft ? (X + W - FillW) : X;
    DrawRect(Fill, FillX, Y, FillW, H);
    DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.18f), FillX, Y, FillW, H * 0.35f);
    if (!Label.IsEmpty())
    {
        DrawShadowText(Label, FLinearColor::White, bRightToLeft ? X + W : X, Y - 22.f, 1.0f, false);
    }
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

    if (ALastBellArenaGameMode* Arena = Cast<ALastBellArenaGameMode>(GM))
    {
        switch (Arena->GetScreen())
        {
        case EArenaScreen::MainMenu:        DrawMainMenu(Arena->GetMenuIndex());       return;
        case EArenaScreen::OpponentSelect:  DrawOpponentSelect(Arena->GetSelectIndex()); return;
        case EArenaScreen::Fighting:        DrawFightHUD(GM);                          return;
        case EArenaScreen::Result:          DrawFightHUD(GM); DrawResultBanner(GM->IsPlayerWinner()); return;
        }
        return;
    }

    // Non-arena (authored) game mode: just the fight HUD.
    DrawFightHUD(GM);
}

void ALastBellHUD::DrawFightHUD(ALastBellGameMode* GM)
{
    const float DT = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.016f;
    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;
    const float Margin = ScreenW * 0.035f;
    const float BarW = ScreenW * 0.36f;
    const float BarH = 26.f;
    const float TopY = 54.f;

    AActor* Player = Cast<AActor>(GM->PlayerBoxer);
    AActor* Opponent = Cast<AActor>(GM->AIBoxerRef);

    const FLinearColor HealthCol(0.88f, 0.16f, 0.16f);
    const FLinearColor StamCol(0.95f, 0.78f, 0.12f);

    if (Player && Player->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        const float HP = IBoxerInterface::Execute_GetHealthPercent(Player);
        const float ST = IBoxerInterface::Execute_GetStaminaPercent(Player);
        DrawPanelBar(Margin, TopY, BarW, BarH, HP, HealthCol, false, TEXT("YOU"));
        DrawPanelBar(Margin, TopY + BarH + 8.f, BarW, 12.f, ST, StamCol, false, FString());
        if (HP <= 0.f && LastPlayerHealth > 0.f) KOFlashTimer = 0.45f;
        LastPlayerHealth = HP;
    }

    if (Opponent && Opponent->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        const float HP = IBoxerInterface::Execute_GetHealthPercent(Opponent);
        const float ST = IBoxerInterface::Execute_GetStaminaPercent(Opponent);
        const float RX = ScreenW - Margin - BarW;
        FString OppName = TEXT("OPPONENT");
        if (ABoxerCharacter* OppChar = Cast<ABoxerCharacter>(Opponent))
        {
            if (OppChar->GetFighterData()) OppName = OppChar->GetFighterData()->FighterName.ToString().ToUpper();
        }
        DrawPanelBar(RX, TopY, BarW, BarH, HP, HealthCol, true, FString());
        DrawShadowText(OppName, FLinearColor::White, RX + BarW, TopY - 22.f, 1.0f, false);
        DrawPanelBar(RX, TopY + BarH + 8.f, BarW, 12.f, ST, StamCol, true, FString());
        if (HP <= 0.f && LastOppHealth > 0.f) KOFlashTimer = 0.45f;
        LastOppHealth = HP;
    }

    const int32 TimeLeft = FMath::Max(0, FMath::CeilToInt(GM->GetRoundTimeRemaining()));
    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), ScreenW * 0.5f - 52.f, TopY - 8.f, 104.f, 64.f);
    DrawShadowText(FString::Printf(TEXT("%02d"), TimeLeft), FLinearColor::White, ScreenW * 0.5f, TopY - 4.f, 2.4f, true);
    DrawShadowText(FString::Printf(TEXT("ROUND %d / %d"), FMath::Max(1, GM->GetCurrentRound()), GM->GetTotalRounds()),
        FLinearColor(0.9f, 0.9f, 0.9f), ScreenW * 0.5f, TopY + 56.f, 0.9f, true);

    if (ABoxerCharacter* PChar = Cast<ABoxerCharacter>(Player))
    {
        if (PChar->ComboComponent)
        {
            const int32 Combo = PChar->ComboComponent->GetCurrentCombo();
            if (Combo > LastComboShown && Combo >= 2) ComboPulse = 1.f;
            LastComboShown = Combo;
            if (Combo >= 2)
            {
                const float Scale = 1.5f + ComboPulse * 0.6f;
                DrawShadowText(FString::Printf(TEXT("%d  HIT  COMBO"), Combo),
                    FLinearColor(1.f, 0.55f + ComboPulse * 0.3f, 0.1f), Margin, TopY + 86.f, Scale, false);
            }
        }
    }
    ComboPulse = FMath::FInterpTo(ComboPulse, 0.f, DT, 5.f);

    if (GM->GetMatchState() == EMatchState::Knockdown)
    {
        const int32 Count = GM->GetKnockdownCount();
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.5f), 0.f, ScreenH * 0.32f, ScreenW, ScreenH * 0.36f);
        DrawShadowText(FString::Printf(TEXT("%d"), Count), FLinearColor(1.f, 0.85f, 0.2f),
            ScreenW * 0.5f, ScreenH * 0.36f, 6.0f, true);
        DrawShadowText(TEXT("DOWN!"), FLinearColor(1.f, 0.3f, 0.2f), ScreenW * 0.5f, ScreenH * 0.30f, 1.6f, true);
    }

    if (KOFlashTimer > 0.f)
    {
        const float Alpha = FMath::Clamp(KOFlashTimer / 0.45f, 0.f, 1.f);
        DrawRect(FLinearColor(1.f, 1.f, 1.f, Alpha * 0.8f), 0.f, 0.f, ScreenW, ScreenH);
        KOFlashTimer -= DT;
    }
}

void ALastBellHUD::DrawMainMenu(int32 MenuIndex)
{
    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.45f), 0.f, 0.f, ScreenW, ScreenH);
    DrawShadowText(TEXT("LAST  BELL"), FLinearColor(1.f, 0.85f, 0.2f), ScreenW * 0.5f, ScreenH * 0.18f, 5.0f, true);
    DrawShadowText(TEXT("ARCADE  BOXING"), FLinearColor(0.85f, 0.85f, 0.9f), ScreenW * 0.5f, ScreenH * 0.30f, 1.4f, true);

    const TCHAR* Options[2] = { TEXT("START"), TEXT("QUIT") };
    for (int32 i = 0; i < 2; ++i)
    {
        const bool bSel = (i == MenuIndex);
        const FLinearColor Col = bSel ? FLinearColor(1.f, 0.9f, 0.3f) : FLinearColor(0.7f, 0.7f, 0.75f);
        const FString Text = bSel ? FString::Printf(TEXT("> %s <"), Options[i]) : FString(Options[i]);
        DrawShadowText(Text, Col, ScreenW * 0.5f, ScreenH * (0.46f + i * 0.10f), 2.0f, true);
    }

    DrawShadowText(TEXT("UP / DOWN  select        ENTER  confirm"),
        FLinearColor(0.7f, 0.7f, 0.75f), ScreenW * 0.5f, ScreenH * 0.85f, 1.0f, true);
}

void ALastBellHUD::DrawOpponentSelect(int32 SelectIndex)
{
    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    const TCHAR* Names[3] = {
        TEXT("REX  'ROOKIE'  RAMONE"),
        TEXT("SAL  'SLIP'  CORRALES"),
        TEXT("BRUNO  'THE BOULDER'  MACK")
    };
    const TCHAR* Descs[3] = {
        TEXT("Rookie  -  slow, predictable, forgiving"),
        TEXT("Counter Boxer  -  dodges and counters"),
        TEXT("Heavyweight  -  slow but devastating power")
    };

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), 0.f, 0.f, ScreenW, ScreenH);
    DrawShadowText(TEXT("SELECT  OPPONENT"), FLinearColor(1.f, 0.85f, 0.2f), ScreenW * 0.5f, ScreenH * 0.16f, 2.6f, true);

    for (int32 i = 0; i < 3; ++i)
    {
        const bool bSel = (i == SelectIndex);
        const float RowY = ScreenH * (0.36f + i * 0.14f);
        if (bSel)
        {
            DrawRect(FLinearColor(0.9f, 0.7f, 0.1f, 0.18f), ScreenW * 0.18f, RowY - 6.f, ScreenW * 0.64f, ScreenH * 0.11f);
        }
        const FLinearColor NameCol = bSel ? FLinearColor(1.f, 0.92f, 0.35f) : FLinearColor(0.8f, 0.8f, 0.85f);
        DrawShadowText(FString::Printf(TEXT("%s%s"), bSel ? TEXT("> ") : TEXT("   "), Names[i]),
            NameCol, ScreenW * 0.5f, RowY, 1.6f, true);
        DrawShadowText(Descs[i], FLinearColor(0.7f, 0.7f, 0.75f), ScreenW * 0.5f, RowY + ScreenH * 0.05f, 0.95f, true);
    }

    DrawShadowText(TEXT("UP / DOWN  select        ENTER  fight        ESC  back"),
        FLinearColor(0.7f, 0.7f, 0.75f), ScreenW * 0.5f, ScreenH * 0.88f, 1.0f, true);
}

void ALastBellHUD::DrawResultBanner(bool bPlayerWon)
{
    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), 0.f, 0.f, ScreenW, ScreenH);
    DrawShadowText(bPlayerWon ? TEXT("YOU WIN") : TEXT("YOU LOSE"),
        bPlayerWon ? FLinearColor(1.f, 0.84f, 0.2f) : FLinearColor(0.9f, 0.2f, 0.2f),
        ScreenW * 0.5f, ScreenH * 0.36f, 4.5f, true);
    DrawShadowText(TEXT("R  rematch        ENTER / ESC  menu"), FLinearColor::White,
        ScreenW * 0.5f, ScreenH * 0.54f, 1.3f, true);
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
