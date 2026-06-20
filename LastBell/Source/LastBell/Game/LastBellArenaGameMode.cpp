#include "Game/LastBellArenaGameMode.h"
#include "Game/FighterFactory.h"
#include "Game/LastBellPlayerController.h"
#include "Characters/PlayerBoxer.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "AI/BoxingAIController.h"
#include "Data/FighterDataAsset.h"
#include "Environment/BoxingRingActor.h"
#include "Environment/CrowdActor.h"
#include "UI/LastBellHUD.h"
#include "Characters/BoxerCharacter.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

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

    // The engine has already spawned the player pawn via DefaultPawnClass.
    APlayerBoxer* Player = Cast<APlayerBoxer>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (Player)
    {
        Player->SetActorLocation(FVector(-160.f, 0.f, 130.f));
        Player->SetActorRotation(FRotator(0.f, 0.f, 0.f));
        Player->LoadFighterData(UFighterFactory::MakePlayer(this));
    }

    // Build and place the opponent.
    AAIBoxer* AI = SpawnAIBoxer(FVector(160.f, 0.f, 130.f));
    CurrentOpponentData = UFighterFactory::MakeOpponent(this, OpponentType);
    if (AI && CurrentOpponentData)
    {
        AI->LoadFighterData(CurrentOpponentData);
        if (AI->PersonalityComponent)
        {
            AI->PersonalityComponent->Initialize(CurrentOpponentData->Personality);
        }
    }

    // StartMatch discovers both boxers, links them as opponents, and kicks off
    // the round flow (after its built-in intro delay).
    StartMatch(CurrentOpponentData);
}

void ALastBellArenaGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // After a match, wait for the rematch key (R) since there is no menu level.
    if (GetMatchState() == EMatchState::MatchOver)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (PC->WasInputKeyJustPressed(EKeys::R))
            {
                RestartArena();
            }
        }
    }
}

void ALastBellArenaGameMode::HandleMatchEnd(bool bPlayerWon)
{
    // Stay in-world (no win/lose level in the code-only boot): strike victory and
    // defeat poses. The HUD draws the result banner + rematch prompt.
    if (PlayerBoxer) PlayerBoxer->SetEndPose(bPlayerWon);
    if (AIBoxerRef)  AIBoxerRef->SetEndPose(!bPlayerWon);

    if (PlayerBoxer) PlayerBoxer->SetBoxerEnabled(false);
    if (AIBoxerRef)  AIBoxerRef->SetBoxerEnabled(false);
}

void ALastBellArenaGameMode::RestartArena()
{
    if (PlayerBoxer)
    {
        PlayerBoxer->ResetState();
        PlayerBoxer->SetActorLocation(FVector(-160.f, 0.f, 130.f));
    }
    if (AIBoxerRef)
    {
        AIBoxerRef->ResetState();
        AIBoxerRef->SetActorLocation(FVector(160.f, 0.f, 130.f));
    }
    StartMatch(CurrentOpponentData);
}

void ALastBellArenaGameMode::SpawnEnvironment()
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<ABoxingRingActor>(ABoxingRingActor::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator, Params);

    GetWorld()->SpawnActor<ACrowdActor>(ACrowdActor::StaticClass(),
        FVector(0.f, 0.f, 60.f), FRotator::ZeroRotator, Params);
}

void ALastBellArenaGameMode::SpawnLighting()
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Key light + a softer fill from the opposite side so both fighters read clearly
    // even without a SkyLight/atmosphere. The ring actor adds its own corner spots.
    auto SpawnDir = [&](const FRotator& Rot, float Intensity, const FLinearColor& Color)
    {
        if (ADirectionalLight* Light = GetWorld()->SpawnActor<ADirectionalLight>(
            ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 900.f), Rot, Params))
        {
            if (Light->GetLightComponent())
            {
                Light->GetLightComponent()->SetIntensity(Intensity);
                Light->GetLightComponent()->SetLightColor(Color);
            }
        }
    };

    SpawnDir(FRotator(-50.f, -35.f, 0.f), 5.f, FLinearColor(1.f, 0.97f, 0.92f));
    SpawnDir(FRotator(-35.f, 145.f, 0.f), 2.f, FLinearColor(0.7f, 0.78f, 0.95f));
}

AAIBoxer* ALastBellArenaGameMode::SpawnAIBoxer(const FVector& Location)
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AAIBoxer* AI = GetWorld()->SpawnActor<AAIBoxer>(AAIBoxer::StaticClass(),
        Location, FRotator(0.f, 180.f, 0.f), Params);
    return AI;
}
