#include "Animation/BoxerAnimInstance.h"
#include "Characters/BoxerCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/BoxerStatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBoxerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwnerBoxer.IsValid())
    {
        OwnerBoxer = Cast<ABoxerCharacter>(TryGetPawnOwner());
    }

    if (!OwnerBoxer.IsValid()) return;

    ABoxerCharacter* Boxer = OwnerBoxer.Get();

    BoxerState = Boxer->GetCurrentState();
    bIsAttacking = (BoxerState == EBoxerState::Attacking);
    bIsDucking = (BoxerState == EBoxerState::Ducking);
    bIsDodging = (BoxerState == EBoxerState::Dodging);
    bIsKnockedDown = (BoxerState == EBoxerState::KnockedDown);
    bIsKO = (BoxerState == EBoxerState::KO);
    bVictory = (BoxerState == EBoxerState::Victory);
    bDefeat = (BoxerState == EBoxerState::Defeat);
    bInStagger = (BoxerState == EBoxerState::HitStun);

    if (UCombatComponent* Combat = Boxer->CombatComponent)
    {
        AttackPhase = Combat->GetCurrentAttackPhase();
    }

    if (UCharacterMovementComponent* Move = Boxer->GetCharacterMovement())
    {
        FVector Velocity = Move->Velocity;
        MoveSpeed = Velocity.Size();
        HorizontalMovement = FVector::DotProduct(Velocity.GetSafeNormal(),
            Boxer->GetActorRightVector());
        LeanDirection = HorizontalMovement;
    }

    if (UBoxerStatsComponent* Stats = Boxer->StatsComponent)
    {
        float StaminaPct = Stats->GetStaminaPercent();
        float TargetBreath = StaminaPct < 0.4f ?
            FMath::GetMappedRangeValueClamped(FVector2D(0.f, 0.4f), FVector2D(1.f, 0.f), StaminaPct) : 0.f;
        BreathingWeight = FMath::FInterpTo(BreathingWeight, TargetBreath, DeltaSeconds, 2.f);
    }
}
