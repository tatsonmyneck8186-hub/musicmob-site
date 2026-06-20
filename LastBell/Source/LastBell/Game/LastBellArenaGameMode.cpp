#include "Game/LastBellArenaGameMode.h"
#include "Game/FighterFactory.h"
#include "Game/LastBellPlayerController.h"
#include "Characters/PlayerBoxer.h"
#include "Characters/AIBoxer.h"
#include "Characters/BoxerCharacter.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "AI/BoxingAIController.h"
#include "Data/FighterDataAsset.h"
#include "Environment/BoxingRingActor.h"
#include "Environment/CrowdActor.h"
#include "UI/LastBellHUD.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ALastBellArenaGameMode::ALastBellArenaGameMode()
{
    DefaultPawnClass = APlayerBoxer::StaticClass();
    PlayerControllerClass = ALastBellPlayerController::StaticClass();
    HUDClass = ALastBellHUD::StaticClass();
    PrimaryActorTick.bCanEverTick = true;
}

void ALastBellArenaGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (bSpawnEnvironment)
    {
        SpawnEnvironment();
        SpawnLighting();
    }

    // The engine spawns the player pawn via DefaultPawnClass; it serves as the
    // menu backdrop camera and the player fighter.
    PlayerRef = Cast<APlayerBoxer>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (PlayerRef)
    {
        PlayerRef->SetActorLocation(FVector(-160.f, 0.f, 130.f));
        PlayerRef->SetActorRotation(FRotator::ZeroRotator);
        PlayerRef->LoadFighterData(UFighterFactory::MakePlayer(this));
        PlayerRef->SetBoxerEnabled(false);
    }

    EnterMainMenu();
}

EFighterType ALastBellArenaGameMode::TypeForIndex(int32 Index)
{
    switch (Index)
    {
    case 1:  return EFighterType::Counter;
    case 2:  return EFighterType::Heavyweight;
    default: return EFighterType::Rookie;
    }
}

void ALastBellArenaGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    HandleMenuInput();
}

void ALastBellArenaGameMode::HandleMenuInput()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    const bool bUp     = PC->WasInputKeyJustPressed(EKeys::Up) || PC->WasInputKeyJustPressed(EKeys::W);
    const bool bDown   = PC->WasInputKeyJustPressed(EKeys::Down) || PC->WasInputKeyJustPressed(EKeys::S);
    const bool bEnter  = PC->WasInputKeyJustPressed(EKeys::Enter);
    const bool bBack   = PC->WasInputKeyJustPressed(EKeys::Escape) || PC->WasInputKeyJustPressed(EKeys::BackSpace);
    const bool bRematch = PC->WasInputKeyJustPressed(EKeys::R);

    switch (Screen)
    {
    case EArenaScreen::MainMenu:
        if (bUp || bDown) MenuIndex = (MenuIndex + 1) % 2;
        if (bEnter)
        {
            if (MenuIndex == 0) EnterOpponentSelect();
            else UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
        }
        break;

    case EArenaScreen::OpponentSelect:
        if (bUp)   SelectIndex = (SelectIndex + 2) % 3;
        if (bDown) SelectIndex = (SelectIndex + 1) % 3;
        if (bEnter) StartSelectedFight();
        if (bBack)  EnterMainMenu();
        break;

    case EArenaScreen::Fighting:
        if (bBack) ReturnToMenu();
        break;

    case EArenaScreen::Result:
        if (bRematch) StartSelectedFight();
        else if (bEnter || bBack) ReturnToMenu();
        break;
    }
}

void ALastBellArenaGameMode::EnterMainMenu()
{
    AbortMatch();
    Screen = EArenaScreen::MainMenu;
    MenuIndex = 0;

    if (AIBoxer)
    {
        if (ABoxingAIController* AIC = Cast<ABoxingAIController>(AIBoxer->GetController()))
        {
            AIC->StopAI();
        }
        AIBoxer->ResetState();
        AIBoxer->SetActorHiddenInGame(true);
        AIBoxer->SetActorEnableCollision(false);
    }
    if (PlayerRef)
    {
        PlayerRef->ResetState();
        PlayerRef->SetActorLocation(FVector(-160.f, 0.f, 130.f));
        PlayerRef->SetActorRotation(FRotator::ZeroRotator);
        PlayerRef->SetBoxerEnabled(false);
    }
}

void ALastBellArenaGameMode::EnterOpponentSelect()
{
    Screen = EArenaScreen::OpponentSelect;
    SelectIndex = 0;
}

void ALastBellArenaGameMode::StartSelectedFight()
{
    const EFighterType Type = TypeForIndex(SelectIndex);
    CurrentOpponentData = UFighterFactory::MakeOpponent(this, Type);

    const FVector AIPos(160.f, 0.f, 130.f);
    if (!AIBoxer)
    {
        AIBoxer = SpawnAIBoxer(AIPos);
    }
    if (AIBoxer && CurrentOpponentData)
    {
        AIBoxer->SetActorHiddenInGame(false);
        AIBoxer->SetActorEnableCollision(true);
        AIBoxer->ResetState();
        AIBoxer->SetActorLocation(AIPos);
        AIBoxer->SetActorRotation(FRotator(0.f, 180.f, 0.f));
        AIBoxer->LoadFighterData(CurrentOpponentData);
        if (AIBoxer->PersonalityComponent)
        {
            AIBoxer->PersonalityComponent->Initialize(CurrentOpponentData->Personality);
        }
    }
    if (PlayerRef)
    {
        PlayerRef->ResetState();
        PlayerRef->SetActorLocation(FVector(-160.f, 0.f, 130.f));
        PlayerRef->SetActorRotation(FRotator::ZeroRotator);
    }

    Screen = EArenaScreen::Fighting;
    StartMatch(CurrentOpponentData);
}

void ALastBellArenaGameMode::ReturnToMenu()
{
    EnterMainMenu();
}

void ALastBellArenaGameMode::HandleMatchEnd(bool bPlayerWon)
{
    // Stay in-world: strike victory/defeat poses; the HUD draws the result banner.
    if (PlayerBoxer) PlayerBoxer->SetEndPose(bPlayerWon);
    if (AIBoxerRef)  AIBoxerRef->SetEndPose(!bPlayerWon);

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);

    Screen = EArenaScreen::Result;
}

void ALastBellArenaGameMode::SpawnEnvironment()
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<ABoxingRingActor>(ABoxingRingActor::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator, Params);

    GetWorld()->SpawnActor<ACrowdActor>(ACrowdActor::StaticClass(),
        FVector(0.f, 0.f, -60.f), FRotator::ZeroRotator, Params);
}

void ALastBellArenaGameMode::SpawnLighting()
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Key + fill directional lights (the ring adds its own warm corner spots).
    auto SpawnDir = [&](const FRotator& Rot, float Intensity, const FLinearColor& Color, bool bShadows)
    {
        if (ADirectionalLight* Light = GetWorld()->SpawnActor<ADirectionalLight>(
            ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 900.f), Rot, Params))
        {
            if (ULightComponent* LC = Light->GetLightComponent())
            {
                LC->SetIntensity(Intensity);
                LC->SetLightColor(Color);
                LC->SetCastShadows(bShadows);
            }
        }
    };
    SpawnDir(FRotator(-52.f, -35.f, 0.f), 4.5f, FLinearColor(1.f, 0.97f, 0.92f), true);
    SpawnDir(FRotator(-32.f, 150.f, 0.f), 1.6f, FLinearColor(0.65f, 0.72f, 0.95f), false);

    // Colored arena accent lights for an arcade look.
    auto SpawnPoint = [&](const FVector& Loc, const FLinearColor& Color, float Intensity, float Radius)
    {
        if (APointLight* Light = GetWorld()->SpawnActor<APointLight>(
            APointLight::StaticClass(), Loc, FRotator::ZeroRotator, Params))
        {
            if (UPointLightComponent* PC = Cast<UPointLightComponent>(Light->GetLightComponent()))
            {
                PC->SetIntensity(Intensity);
                PC->SetLightColor(Color);
                PC->SetAttenuationRadius(Radius);
                PC->SetCastShadows(false);
            }
        }
    };
    SpawnPoint(FVector(0.f, -750.f, 520.f), FLinearColor(0.2f, 0.4f, 1.f), 50000.f, 1800.f);
    SpawnPoint(FVector(0.f,  750.f, 520.f), FLinearColor(1.f, 0.35f, 0.15f), 50000.f, 1800.f);

    // Haze / atmosphere.
    if (AExponentialHeightFog* Fog = GetWorld()->SpawnActor<AExponentialHeightFog>(
        AExponentialHeightFog::StaticClass(), FVector(0.f, 0.f, -200.f), FRotator::ZeroRotator, Params))
    {
        if (UExponentialHeightFogComponent* FC = Fog->FindComponentByClass<UExponentialHeightFogComponent>())
        {
            FC->SetFogDensity(0.025f);
            FC->SetFogInscatteringColor(FLinearColor(0.05f, 0.06f, 0.12f));
            FC->SetFogHeightFalloff(0.15f);
        }
    }
}

AAIBoxer* ALastBellArenaGameMode::SpawnAIBoxer(const FVector& Location)
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    return GetWorld()->SpawnActor<AAIBoxer>(AAIBoxer::StaticClass(),
        Location, FRotator(0.f, 180.f, 0.f), Params);
}
