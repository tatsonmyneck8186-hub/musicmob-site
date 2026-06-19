#include "Characters/BoxerCharacter.h"
#include "Components/BoxerStatsComponent.h"
#include "Components/CombatComponent.h"
#include "Components/BoxerFeedbackComponent.h"
#include "Components/ComboComponent.h"
#include "Components/BoxerAudioComponent.h"
#include "Data/FighterDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ABoxerCharacter::ABoxerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    StatsComponent = CreateDefaultSubobject<UBoxerStatsComponent>(TEXT("StatsComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    FeedbackComponent = CreateDefaultSubobject<UBoxerFeedbackComponent>(TEXT("FeedbackComponent"));
    ComboComponent = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComponent"));
    AudioComponent = CreateDefaultSubobject<UBoxerAudioComponent>(TEXT("AudioComponent"));

    // Placeholder body: a capsule-ish stack of basic shapes so a code-only boot is
    // visible even with no skeletal mesh assigned. Replaced by SetSkeletalMesh when
    // a fighter with a real mesh is loaded.
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootComponent);
    BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
    BodyMesh->SetRelativeScale3D(FVector(0.9f, 0.9f, 1.75f));
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BodyMesh->SetCastShadow(true);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CylinderMesh.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(
        TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (ShapeMat.Succeeded())
    {
        BodyMesh->SetMaterial(0, ShapeMat.Object);
    }

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABoxerCharacter::BeginPlay()
{
    Super::BeginPlay();

    StatsComponent->OnKnockdown.AddDynamic(this, &ABoxerCharacter::OnKnockdown);
    StatsComponent->OnKO.AddDynamic(this, &ABoxerCharacter::OnKO);
    CombatComponent->OnHitLanded.AddDynamic(this, &ABoxerCharacter::OnHitLanded);
    CombatComponent->OnHitRegistered.AddDynamic(this, &ABoxerCharacter::OnHitRegistered);
    CombatComponent->OnMissRegistered.AddDynamic(this, &ABoxerCharacter::OnMissRegistered);
}

void ABoxerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState != EBoxerState::KnockedDown && CurrentState != EBoxerState::KO)
    {
        UpdateFacingDirection(DeltaTime);
        ClampToRingBounds();
    }
}

void ABoxerCharacter::LoadFighterData(UFighterDataAsset* Data)
{
    if (!Data) return;
    FighterData = Data;

    StatsComponent->InitializeStats(Data->Stats);

    if (Data->FighterMesh)
    {
        GetMesh()->SetSkeletalMesh(Data->FighterMesh);
        // A real fighter mesh is present; the placeholder body is no longer needed.
        BodyMesh->SetVisibility(false);

        UMaterialInstanceDynamic* Mat = GetMesh()->CreateDynamicMaterialInstance(0);
        if (Mat)
        {
            Mat->SetVectorParameterValue("PrimaryColor", Data->PrimaryColor);
            Mat->SetVectorParameterValue("SecondaryColor", Data->SecondaryColor);
        }
    }
    else if (BodyMesh)
    {
        // Code-only boot: tint the placeholder body with the fighter's color.
        BodyMaterial = BodyMesh->CreateDynamicMaterialInstance(0);
        if (BodyMaterial)
        {
            BodyMaterial->SetVectorParameterValue("Color", Data->PrimaryColor);
        }
    }

    if (Data->AnimClass)
    {
        GetMesh()->SetAnimClass(Data->AnimClass);
    }
}

void ABoxerCharacter::SetBoxerEnabled(bool bEnabled)
{
    SetActorTickEnabled(bEnabled);
    CombatComponent->SetEnabled(bEnabled);
    GetCharacterMovement()->SetMovementMode(bEnabled ? MOVE_Walking : MOVE_None);
}

void ABoxerCharacter::ExecuteAttack_Implementation(EBoxingMove Move)
{
    if (!FighterData) return;
    if (!StatsComponent->ConsumeStamina(FighterData->GetAttackData(Move).StaminaCost)) return;
    CurrentState = EBoxerState::Attacking;
    CombatComponent->TryAttack(Move, FighterData->GetAttackData(Move));
}

void ABoxerCharacter::ReceiveHit_Implementation(FAttackData AttackData, AActor* Attacker)
{
    if (CurrentState == EBoxerState::KO || CurrentState == EBoxerState::KnockedDown) return;

    StatsComponent->ApplyDamage(AttackData.Damage);
    StatsComponent->AddKOMeter(AttackData.KOMeterGain);

    CurrentState = EBoxerState::HitStun;
    AudioComponent->PlayHitReaction();
    ComboComponent->BreakCombo();

    GetWorldTimerManager().SetTimer(HitStunHandle,
        [this]() { CurrentState = EBoxerState::Idle; }, 0.3f, false);
}

void ABoxerCharacter::ExecuteDodge_Implementation()
{
    if (CurrentState == EBoxerState::Attacking) return;
    CurrentState = EBoxerState::Dodging;
    CombatComponent->TryDodge();

    GetWorldTimerManager().SetTimer(DodgeHandle,
        [this]() { if (CurrentState == EBoxerState::Dodging) CurrentState = EBoxerState::Idle; },
        CombatComponent->DodgeDuration, false);
}

void ABoxerCharacter::ExecuteDuck_Implementation()
{
    if (CurrentState == EBoxerState::Attacking) return;
    CurrentState = EBoxerState::Ducking;
    CombatComponent->bIsDucking = true;
}

void ABoxerCharacter::StopDuck_Implementation()
{
    CombatComponent->bIsDucking = false;
    if (CurrentState == EBoxerState::Ducking) CurrentState = EBoxerState::Idle;
}

EBoxerState ABoxerCharacter::GetBoxerState_Implementation() const { return CurrentState; }
float ABoxerCharacter::GetHealthPercent_Implementation() const { return StatsComponent->GetHealthPercent(); }
float ABoxerCharacter::GetStaminaPercent_Implementation() const { return StatsComponent->GetStaminaPercent(); }
float ABoxerCharacter::GetKOMeterPercent_Implementation() const { return StatsComponent->GetKOMeterPercent(); }
bool ABoxerCharacter::IsAttacking_Implementation() const { return CurrentState == EBoxerState::Attacking; }
bool ABoxerCharacter::IsDucking_Implementation() const { return CurrentState == EBoxerState::Ducking; }
void ABoxerCharacter::SetOpponent_Implementation(AActor* InOpponent) { OpponentRef = InOpponent; CombatComponent->SetOpponent(InOpponent); }
AActor* ABoxerCharacter::GetOpponent_Implementation() const { return OpponentRef.Get(); }

void ABoxerCharacter::UpdateFacingDirection(float DeltaTime)
{
    if (!OpponentRef.IsValid()) return;
    FVector ToOpponent = (OpponentRef->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    ToOpponent.Z = 0.f;
    if (!ToOpponent.IsNearlyZero())
    {
        FRotator Target = ToOpponent.Rotation();
        FRotator Current = GetActorRotation();
        SetActorRotation(FMath::RInterpTo(Current, Target, DeltaTime, 10.f));
    }
}

void ABoxerCharacter::ClampToRingBounds()
{
    FVector Loc = GetActorLocation();
    Loc.Y = FMath::Clamp(Loc.Y, -RingBoundsY, RingBoundsY);
    SetActorLocation(Loc);
}

void ABoxerCharacter::OnKnockdown()
{
    CurrentState = EBoxerState::KnockedDown;
    AudioComponent->PlayStagger();
}

void ABoxerCharacter::OnKO()
{
    CurrentState = EBoxerState::KO;
    AudioComponent->PlayDefeat();
    FeedbackComponent->TriggerKOSlowMotion();
}

void ABoxerCharacter::OnHitLanded(AActor* Target, FAttackData AttackData)
{
    if (AttackData.bCausesScreenFlash)
    {
        FeedbackComponent->TriggerScreenFlash(AttackData.ScreenFlashIntensity, AttackData.ScreenFlashDuration);
    }
}

void ABoxerCharacter::OnHitRegistered(EBoxingMove Move)
{
    ComboComponent->RegisterHit(Move);
}

void ABoxerCharacter::OnMissRegistered(EBoxingMove Move)
{
    AudioComponent->PlayMissSwish();
}
