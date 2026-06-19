#include "Characters/AIBoxer.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "Components/BoxerStatsComponent.h"
#include "Core/IBoxerInterface.h"
#include "Data/FighterDataAsset.h"
#include "AI/BoxingAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIBoxer::AAIBoxer()
{
    PersonalityComponent = CreateDefaultSubobject<UBoxingAIPersonalityComponent>(TEXT("PersonalityComponent"));
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = ABoxingAIController::StaticClass();
}

void AAIBoxer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!OpponentRef.IsValid() || FMath::IsNearlyZero(ApproachIntent)) return;

    FVector Direction = (OpponentRef->GetActorLocation() - GetActorLocation());
    Direction.Z = 0.f;
    const float Dist = Direction.Size();
    Direction = Direction.GetSafeNormal();
    if (Direction.IsNearlyZero()) return;

    float SpeedMult = PersonalityComponent ? PersonalityComponent->GetAttackFrequencyMultiplier() : 1.f;

    // Advance toward the opponent but don't crowd past the preferred spacing.
    if (ApproachIntent > 0.f && Dist <= PreferredSpacing) return;

    float Sign = ApproachIntent > 0.f ? 1.f : -1.f;
    AddMovementInput(Direction, Sign * SpeedMult);
}

void AAIBoxer::BeginPlay()
{
    Super::BeginPlay();

    if (FighterData)
    {
        PersonalityComponent->Initialize(FighterData->Personality);
    }

    StatsComponent->OnHealthChanged.AddDynamic(this, &AAIBoxer::OnHealthChanged);
}

void AAIBoxer::AIExecuteAttack(EBoxingMove Move)
{
    ExecuteAttack_Implementation(Move);
    PersonalityComponent->OnPunchLanded();
}

void AAIBoxer::AIExecuteDodge()
{
    ExecuteDodge_Implementation();
}

void AAIBoxer::AIMoveTowardPlayer(float DeltaTime)
{
    if (!OpponentRef.IsValid()) return;
    FVector Direction = (OpponentRef->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    Direction.Z = 0.f;
    float SpeedMult = PersonalityComponent->GetAttackFrequencyMultiplier();
    AddMovementInput(Direction, SpeedMult);
}

void AAIBoxer::AIMoveAwayFromPlayer(float DeltaTime)
{
    if (!OpponentRef.IsValid()) return;
    FVector Direction = (GetActorLocation() - OpponentRef->GetActorLocation()).GetSafeNormal();
    Direction.Z = 0.f;
    AddMovementInput(Direction, 1.f);
}

float AAIBoxer::GetDistanceToOpponent() const
{
    if (!OpponentRef.IsValid()) return 9999.f;
    return FVector::Dist(GetActorLocation(), OpponentRef->GetActorLocation());
}

bool AAIBoxer::IsOpponentAttacking() const
{
    if (!OpponentRef.IsValid()) return false;
    AActor* Opp = OpponentRef.Get();
    if (Opp->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass()))
    {
        return IBoxerInterface::Execute_IsAttacking(Opp);
    }
    return false;
}

bool AAIBoxer::IsInAttackRange() const
{
    return GetDistanceToOpponent() <= AttackRange;
}

void AAIBoxer::OnHealthChanged(float NewHealth, float MaxHealth)
{
    float Percent = MaxHealth > 0.f ? NewHealth / MaxHealth : 0.f;
    PersonalityComponent->OnHealthUpdated(Percent);
}
