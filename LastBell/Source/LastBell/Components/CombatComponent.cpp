#include "Components/CombatComponent.h"
#include "Components/BoxerStatsComponent.h"
#include "Core/IBoxerInterface.h"
#include "GameFramework/Character.h"
#include "Engine/WorldSettings.h"
#include "HAL/PlatformTime.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bInHitPause)
    {
        TickHitPause();
    }

    if (bIsAttacking)
    {
        TickAttack(DeltaTime);
    }

    if (bIsDodging)
    {
        TickDodge(DeltaTime);
    }
}

bool UCombatComponent::TryAttack(EBoxingMove Move, const FAttackData& AttackData)
{
    if (!bCombatEnabled || bIsAttacking || bIsDodging) return false;

    bIsAttacking = true;
    bIsFakeAttack = false;
    bHitRegisteredThisAttack = false;
    AttackElapsed = 0.f;
    CurrentAttackData = AttackData;
    CurrentAttackData.MoveType = Move;

    HitboxActiveStart = AttackData.StartupDuration;
    HitboxActiveEnd = AttackData.StartupDuration + AttackData.ActiveDuration;
    AttackTotalDuration = AttackData.StartupDuration + AttackData.ActiveDuration + AttackData.RecoveryDuration;

    OnAttackExecuted.Broadcast(Move);
    OnAttackStateChanged.Broadcast(EAttackPhase::Startup);

    return true;
}

bool UCombatComponent::TryFakePunch(EBoxingMove Move, const FAttackData& AttackData)
{
    if (!bCombatEnabled || bIsAttacking || bIsDodging) return false;

    bIsAttacking = true;
    bIsFakeAttack = true;
    bHitRegisteredThisAttack = false;
    AttackElapsed = 0.f;
    CurrentAttackData = AttackData;
    CurrentAttackData.MoveType = Move;

    HitboxActiveStart = AttackData.StartupDuration;
    HitboxActiveEnd = AttackData.StartupDuration * 0.8f;
    AttackTotalDuration = AttackData.StartupDuration + AttackData.RecoveryDuration * 0.5f;

    OnAttackExecuted.Broadcast(Move);
    OnAttackStateChanged.Broadcast(EAttackPhase::Startup);

    return true;
}

void UCombatComponent::TryDodge()
{
    if (!bCombatEnabled || bIsDodging || bIsAttacking) return;
    bIsDodging = true;
    DodgeElapsed = 0.f;
}

EAttackPhase UCombatComponent::GetCurrentAttackPhase() const
{
    if (!bIsAttacking) return EAttackPhase::None;
    if (AttackElapsed < HitboxActiveStart) return EAttackPhase::Startup;
    if (AttackElapsed < HitboxActiveEnd) return EAttackPhase::Active;
    return EAttackPhase::Recovery;
}

void UCombatComponent::TickAttack(float DeltaTime)
{
    AttackElapsed += DeltaTime;

    EAttackPhase CurrentPhase = GetCurrentAttackPhase();

    if (!bIsFakeAttack && AttackElapsed >= HitboxActiveStart && AttackElapsed < HitboxActiveEnd)
    {
        OnAttackStateChanged.Broadcast(EAttackPhase::Active);
        if (!bHitRegisteredThisAttack)
        {
            CheckHit();
        }
    }
    else if (AttackElapsed >= HitboxActiveStart && !bIsFakeAttack)
    {
        OnAttackStateChanged.Broadcast(EAttackPhase::Recovery);
    }

    if (AttackElapsed >= AttackTotalDuration)
    {
        EndAttack();
    }
}

void UCombatComponent::TickDodge(float DeltaTime)
{
    DodgeElapsed += DeltaTime;
    if (DodgeElapsed >= DodgeDuration)
    {
        bIsDodging = false;
    }
}

void UCombatComponent::TickHitPause()
{
    double Now = FPlatformTime::Seconds();
    if (Now - HitPauseStartRealTime >= HitPauseRealDuration)
    {
        bInHitPause = false;
        if (UWorld* World = GetWorld())
        {
            AWorldSettings* WS = World->GetWorldSettings();
            if (WS) WS->SetTimeDilation(1.f);
        }
    }
}

void UCombatComponent::CheckHit()
{
    if (!Opponent.IsValid()) return;
    if (!IsOpponentInRange()) return;

    AActor* OpponentActor = Opponent.Get();

    if (!OpponentActor->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass())) return;

    if (CanOpponentEvade(CurrentAttackData.MoveType))
    {
        OnMissRegistered.Broadcast(CurrentAttackData.MoveType);
        return;
    }

    bHitRegisteredThisAttack = true;
    OnHitRegistered.Broadcast(CurrentAttackData.MoveType);

    IBoxerInterface::Execute_ReceiveHit(OpponentActor, CurrentAttackData, GetOwner());
    OnHitLanded.Broadcast(OpponentActor, CurrentAttackData);

    if (ACharacter* HitChar = Cast<ACharacter>(OpponentActor))
    {
        FVector Direction = (OpponentActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
        Direction.Z = CurrentAttackData.KnockbackVerticalRatio;
        HitChar->LaunchCharacter(Direction * CurrentAttackData.KnockbackForce, true, true);
    }

    StartHitPause(CurrentAttackData.HitPauseDuration);
}

void UCombatComponent::EndAttack()
{
    bIsAttacking = false;
    bIsFakeAttack = false;

    if (!bHitRegisteredThisAttack)
    {
        OnMissRegistered.Broadcast(CurrentAttackData.MoveType);
    }

    OnAttackStateChanged.Broadcast(EAttackPhase::None);
}

void UCombatComponent::StartHitPause(float RealDuration)
{
    if (UWorld* World = GetWorld())
    {
        AWorldSettings* WS = World->GetWorldSettings();
        if (WS) WS->SetTimeDilation(0.05f);
    }
    bInHitPause = true;
    HitPauseStartRealTime = FPlatformTime::Seconds();
    HitPauseRealDuration = RealDuration;
}

bool UCombatComponent::IsOpponentInRange() const
{
    if (!Opponent.IsValid()) return false;
    float Dist = FVector::Dist(GetOwner()->GetActorLocation(), Opponent->GetActorLocation());
    return Dist <= CurrentAttackData.Range * 1.3f;
}

bool UCombatComponent::IsOpponentInAngle() const
{
    if (!Opponent.IsValid()) return false;
    FVector ToOpponent = (Opponent->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
    float Dot = FVector::DotProduct(GetOwner()->GetActorForwardVector(), ToOpponent);
    return Dot > 0.3f;
}

bool UCombatComponent::CanOpponentEvade(EBoxingMove Move) const
{
    if (!Opponent.IsValid()) return false;
    AActor* OppActor = Opponent.Get();
    if (!OppActor->GetClass()->ImplementsInterface(UBoxerInterface::StaticClass())) return false;

    bool bOppDodging = IBoxerInterface::Execute_GetBoxerState(OppActor) == EBoxerState::Dodging;
    bool bOppDucking = IBoxerInterface::Execute_IsDucking(OppActor);

    if (bOppDodging) return true;
    if (bOppDucking && Move != EBoxingMove::Uppercut) return true;

    return false;
}
