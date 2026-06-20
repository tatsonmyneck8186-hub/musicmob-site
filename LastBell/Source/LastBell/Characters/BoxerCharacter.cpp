#include "Characters/BoxerCharacter.h"
#include "Components/BoxerStatsComponent.h"
#include "Components/CombatComponent.h"
#include "Components/BoxerFeedbackComponent.h"
#include "Components/ComboComponent.h"
#include "Components/BoxerAudioComponent.h"
#include "Data/FighterDataAsset.h"
#include "Environment/ImpactSparkActor.h"
#include "Environment/CrowdActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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

    BuildBody();

    ImpactSparkClass = AImpactSparkActor::StaticClass();

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

UStaticMeshComponent* ABoxerCharacter::MakePart(const FName& Name, USceneComponent* Parent)
{
    UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
    Part->SetupAttachment(Parent);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Part->SetCastShadow(true);
    return Part;
}

void ABoxerCharacter::BuildBody()
{
    // Basic-shape placeholder humanoid: a readable boxing silhouette built entirely
    // from engine primitives so it works code-only with no skeletal mesh.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

    UStaticMesh* Cube = CubeMesh.Succeeded() ? CubeMesh.Object : nullptr;
    UStaticMesh* Sphere = SphereMesh.Succeeded() ? SphereMesh.Object : nullptr;
    UStaticMesh* Cyl = CylinderMesh.Succeeded() ? CylinderMesh.Object : nullptr;
    UMaterialInterface* Mat = ShapeMat.Succeeded() ? ShapeMat.Object : nullptr;

    BodyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BodyRoot"));
    BodyRoot->SetupAttachment(RootComponent);

    auto Setup = [&](UStaticMeshComponent* P, UStaticMesh* M, FVector Loc, FVector Scale)
    {
        if (M) P->SetStaticMesh(M);
        if (Mat) P->SetMaterial(0, Mat);
        P->SetRelativeLocation(Loc);
        P->SetRelativeScale3D(Scale);
    };

    Pelvis = MakePart(TEXT("Pelvis"), BodyRoot);
    Setup(Pelvis, Cube, FVector(0, 0, -8), FVector(0.36f, 0.52f, 0.30f));

    Torso = MakePart(TEXT("Torso"), BodyRoot);
    Setup(Torso, Cube, FVector(0, 0, 24), FVector(0.36f, 0.54f, 0.52f));

    Head = MakePart(TEXT("Head"), Torso);
    Setup(Head, Sphere, FVector(2, 0, 34), FVector(0.26f, 0.26f, 0.28f));

    LeftShoulder = MakePart(TEXT("LeftShoulder"), Torso);
    Setup(LeftShoulder, Cube, FVector(0, 30, 14), FVector(0.18f, 0.18f, 0.34f));

    RightShoulder = MakePart(TEXT("RightShoulder"), Torso);
    Setup(RightShoulder, Cube, FVector(0, -30, 14), FVector(0.18f, 0.18f, 0.34f));

    LeftGlove = MakePart(TEXT("LeftGlove"), Torso);
    Setup(LeftGlove, Sphere, FVector(16, 18, 16), FVector(0.19f, 0.19f, 0.19f));

    RightGlove = MakePart(TEXT("RightGlove"), Torso);
    Setup(RightGlove, Sphere, FVector(16, -18, 16), FVector(0.19f, 0.19f, 0.19f));

    LeftLeg = MakePart(TEXT("LeftLeg"), BodyRoot);
    Setup(LeftLeg, Cyl, FVector(0, 13, -52), FVector(0.18f, 0.18f, 0.5f));

    RightLeg = MakePart(TEXT("RightLeg"), BodyRoot);
    Setup(RightLeg, Cyl, FVector(0, -13, -52), FVector(0.18f, 0.18f, 0.5f));
}

void ABoxerCharacter::TintBody()
{
    auto Tint = [&](UStaticMeshComponent* P, const FLinearColor& C) -> UMaterialInstanceDynamic*
    {
        if (!P) return nullptr;
        UMaterialInstanceDynamic* MID = P->CreateDynamicMaterialInstance(0);
        if (MID) MID->SetVectorParameterValue(TEXT("Color"), C);
        return MID;
    };

    const FLinearColor Primary = FighterData ? FighterData->PrimaryColor : FLinearColor(0.2f, 0.5f, 1.f);
    const FLinearColor Secondary = FighterData ? FighterData->SecondaryColor : FLinearColor(0.1f, 0.1f, 0.1f);
    const FLinearColor Skin(0.78f, 0.58f, 0.46f);
    const FLinearColor Glove = (Primary * 1.35f).GetClamped();

    BodyMat = Tint(Torso, Primary);
    Tint(LeftShoulder, Primary);
    Tint(RightShoulder, Primary);

    TrunkMat = Tint(Pelvis, Secondary);
    Tint(LeftLeg, Secondary);
    Tint(RightLeg, Secondary);

    SkinMat = Tint(Head, Skin);

    GloveMat = Tint(LeftGlove, Glove);
    Tint(RightGlove, Glove);
}

void ABoxerCharacter::BeginPlay()
{
    Super::BeginPlay();

    StatsComponent->OnKnockdown.AddDynamic(this, &ABoxerCharacter::OnKnockdown);
    StatsComponent->OnKO.AddDynamic(this, &ABoxerCharacter::OnKO);
    CombatComponent->OnHitLanded.AddDynamic(this, &ABoxerCharacter::OnHitLanded);
    CombatComponent->OnHitRegistered.AddDynamic(this, &ABoxerCharacter::OnHitRegistered);
    CombatComponent->OnMissRegistered.AddDynamic(this, &ABoxerCharacter::OnMissRegistered);
    CombatComponent->OnAttackStateChanged.AddDynamic(this, &ABoxerCharacter::OnAttackPhaseChanged);
}

void ABoxerCharacter::OnAttackPhaseChanged(EAttackPhase NewPhase)
{
    // When an attack fully ends, drop back to Idle (unless we got interrupted into
    // hit-stun / knockdown / a match-end pose in the meantime).
    if (NewPhase == EAttackPhase::None && CurrentState == EBoxerState::Attacking)
    {
        CurrentState = EBoxerState::Idle;
    }
}

void ABoxerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState != EBoxerState::KnockedDown && CurrentState != EBoxerState::KO &&
        CurrentState != EBoxerState::Victory && CurrentState != EBoxerState::Defeat)
    {
        UpdateFacingDirection(DeltaTime);
        ClampToRingBounds();
    }

    UpdateProceduralAnim(DeltaTime);
}

void ABoxerCharacter::UpdateProceduralAnim(float DeltaTime)
{
    if (!BodyRoot || !Torso || !LeftGlove || !RightGlove) return;

    AnimTime += DeltaTime;
    const float Bob = FMath::Sin(AnimTime * 2.2f) * 1.2f;

    // Default = guard.
    FVector LT(16.f, 18.f, 16.f + Bob);
    FVector RT(16.f, -18.f, 16.f + Bob);
    FRotator TorsoTgt(0.f, 0.f, 0.f);
    FVector BodyLoc(0.f, 0.f, 0.f);
    FRotator BodyRot(0.f, 0.f, 0.f);
    float GloveSpeed = 13.f;

    switch (CurrentState)
    {
    case EBoxerState::Attacking:
    {
        const EAttackPhase Ph = CombatComponent->GetCurrentAttackPhase();
        const bool bActive = (Ph == EAttackPhase::Active);
        const bool bStartup = (Ph == EAttackPhase::Startup);
        if (LastMove == EBoxingMove::Jab)
        {
            if (bActive) { LT = FVector(52.f, 8.f, 18.f); GloveSpeed = 30.f; }
            else if (bStartup) { LT = FVector(4.f, 22.f, 16.f); }
        }
        else if (LastMove == EBoxingMove::Hook)
        {
            if (bActive) { LT = FVector(44.f, -10.f, 18.f); GloveSpeed = 28.f; }
            else if (bStartup) { LT = FVector(4.f, 26.f, 18.f); }
        }
        else if (LastMove == EBoxingMove::Uppercut)
        {
            if (bActive) { RT = FVector(40.f, -8.f, 42.f); GloveSpeed = 28.f; }
            else if (bStartup) { RT = FVector(14.f, -22.f, -6.f); }
        }
        break;
    }
    case EBoxerState::Ducking:
        BodyLoc.Z = -34.f;
        LT.Z -= 12.f; RT.Z -= 12.f;
        break;
    case EBoxerState::Dodging:
        TorsoTgt = FRotator(0.f, 0.f, DodgeDir * 20.f);
        BodyLoc.Y = DodgeDir * 16.f;
        break;
    case EBoxerState::HitStun:
        TorsoTgt = FRotator(-12.f, 0.f, 0.f);
        LT = FVector(8.f, 20.f, 10.f); RT = FVector(8.f, -20.f, 10.f);
        break;
    case EBoxerState::KnockedDown:
    case EBoxerState::KO:
        BodyRot = FRotator(80.f, 0.f, 0.f);   // tip onto back (sign is visual-tunable)
        BodyLoc.Z = -26.f;
        LT = FVector(0.f, 22.f, -2.f); RT = FVector(0.f, -22.f, -2.f);
        GloveSpeed = 8.f;
        break;
    case EBoxerState::Victory:
        LT = FVector(8.f, 18.f, 64.f); RT = FVector(8.f, -18.f, 64.f);
        break;
    case EBoxerState::Defeat:
        BodyRot = FRotator(52.f, 0.f, 0.f);
        BodyLoc.Z = -10.f;
        LT = FVector(12.f, 18.f, -2.f); RT = FVector(12.f, -18.f, -2.f);
        break;
    default:
        break; // Idle / Moving keep guard
    }

    LeftGlove->SetRelativeLocation(FMath::VInterpTo(LeftGlove->GetRelativeLocation(), LT, DeltaTime, GloveSpeed));
    RightGlove->SetRelativeLocation(FMath::VInterpTo(RightGlove->GetRelativeLocation(), RT, DeltaTime, GloveSpeed));
    Torso->SetRelativeRotation(FMath::RInterpTo(Torso->GetRelativeRotation(), TorsoTgt, DeltaTime, 11.f));
    BodyRoot->SetRelativeLocation(FMath::VInterpTo(BodyRoot->GetRelativeLocation(), BodyLoc, DeltaTime, 8.f));
    BodyRoot->SetRelativeRotation(FMath::RInterpTo(BodyRoot->GetRelativeRotation(), BodyRot, DeltaTime, 7.f));
}

void ABoxerCharacter::LoadFighterData(UFighterDataAsset* Data)
{
    if (!Data) return;
    FighterData = Data;

    StatsComponent->InitializeStats(Data->Stats);

    if (Data->FighterMesh)
    {
        // A real skeletal fighter is supplied — use it and hide the placeholder rig.
        GetMesh()->SetSkeletalMesh(Data->FighterMesh);
        if (BodyRoot) BodyRoot->SetVisibility(false, true);

        UMaterialInstanceDynamic* SkelMat = GetMesh()->CreateDynamicMaterialInstance(0);
        if (SkelMat)
        {
            SkelMat->SetVectorParameterValue(TEXT("PrimaryColor"), Data->PrimaryColor);
            SkelMat->SetVectorParameterValue(TEXT("SecondaryColor"), Data->SecondaryColor);
        }
        if (Data->AnimClass) GetMesh()->SetAnimClass(Data->AnimClass);
        return;
    }

    // Placeholder rig: distinct silhouette per archetype + team colors.
    if (BodyRoot)
    {
        FVector Build(1.f, 1.f, 1.f);
        switch (Data->FighterType)
        {
        case EFighterType::Heavyweight: Build = FVector(1.35f, 1.45f, 1.02f); break;
        case EFighterType::Counter:     Build = FVector(0.90f, 0.88f, 1.07f); break;
        case EFighterType::Rookie:      Build = FVector(1.06f, 1.06f, 0.98f); break;
        default:                        Build = FVector(1.0f, 1.0f, 1.0f);    break;
        }
        BodyRoot->SetRelativeScale3D(Build);
    }
    TintBody();
}

void ABoxerCharacter::SetEndPose(bool bWon)
{
    bInEndPose = true;
    bEndPoseWon = bWon;
    CurrentState = bWon ? EBoxerState::Victory : EBoxerState::Defeat;
    if (AudioComponent)
    {
        if (bWon) AudioComponent->PlayVictory();
        else AudioComponent->PlayDefeat();
    }
}

void ABoxerCharacter::ResetState()
{
    CurrentState = EBoxerState::Idle;
    bInEndPose = false;
    if (CombatComponent) CombatComponent->bIsDucking = false;
    GetWorldTimerManager().ClearTimer(HitStunHandle);
    GetWorldTimerManager().ClearTimer(DodgeHandle);
}

void ABoxerCharacter::SetBoxerEnabled(bool bEnabled)
{
    SetActorTickEnabled(true); // keep ticking so the body keeps animating
    CombatComponent->SetEnabled(bEnabled);
    GetCharacterMovement()->SetMovementMode(bEnabled ? MOVE_Walking : MOVE_None);
}

void ABoxerCharacter::ExecuteAttack_Implementation(EBoxingMove Move)
{
    if (!FighterData) return;
    if (CurrentState == EBoxerState::Victory || CurrentState == EBoxerState::Defeat) return;
    if (!CombatComponent->CanStartAttack()) return;
    const FAttackData AD = FighterData->GetAttackData(Move);
    if (!StatsComponent->ConsumeStamina(AD.StaminaCost)) return;
    LastMove = Move;
    CurrentState = EBoxerState::Attacking;
    CombatComponent->TryAttack(Move, AD);
}

void ABoxerCharacter::ReceiveHit_Implementation(FAttackData AttackData, AActor* Attacker)
{
    if (CurrentState == EBoxerState::KO || CurrentState == EBoxerState::KnockedDown) return;
    if (CurrentState == EBoxerState::Victory || CurrentState == EBoxerState::Defeat) return;

    StatsComponent->ApplyDamage(AttackData.Damage);
    StatsComponent->AddKOMeter(AttackData.KOMeterGain);

    if (CurrentState == EBoxerState::KO || CurrentState == EBoxerState::KnockedDown) return;

    CurrentState = EBoxerState::HitStun;
    AudioComponent->PlayHitReaction();
    ComboComponent->BreakCombo();

    GetWorldTimerManager().SetTimer(HitStunHandle,
        [this]() { if (CurrentState == EBoxerState::HitStun) CurrentState = EBoxerState::Idle; }, 0.3f, false);
}

void ABoxerCharacter::ExecuteDodge_Implementation()
{
    if (!CombatComponent->IsCombatEnabled()) return;
    if (CurrentState == EBoxerState::Attacking) return;
    if (CurrentState == EBoxerState::Victory || CurrentState == EBoxerState::Defeat) return;
    DodgeDir = FMath::RandBool() ? 1.f : -1.f;
    CurrentState = EBoxerState::Dodging;
    CombatComponent->TryDodge();

    GetWorldTimerManager().SetTimer(DodgeHandle,
        [this]() { if (CurrentState == EBoxerState::Dodging) CurrentState = EBoxerState::Idle; },
        CombatComponent->DodgeDuration, false);
}

void ABoxerCharacter::ExecuteDuck_Implementation()
{
    if (!CombatComponent->IsCombatEnabled()) return;
    if (CurrentState == EBoxerState::Attacking) return;
    if (CurrentState == EBoxerState::Victory || CurrentState == EBoxerState::Defeat) return;
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

    // Big world flash at the knockout.
    SpawnImpactSpark(GetActorLocation() + FVector(0.f, 0.f, 60.f), FLinearColor(1.f, 0.95f, 0.8f), 1.f, true);
    if (ACrowdActor* Crowd = FindCrowd()) Crowd->ReactToHeavyHit();
}

void ABoxerCharacter::OnHitLanded(AActor* Target, FAttackData AttackData)
{
    if (AttackData.bCausesScreenFlash)
    {
        FeedbackComponent->TriggerScreenFlash(AttackData.ScreenFlashIntensity, AttackData.ScreenFlashDuration);
    }

    if (Target)
    {
        const bool bHeavy = AttackData.bCausesScreenFlash || AttackData.Damage >= 15.f;
        const FVector ToSelf = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
        const float ImpactZ = (AttackData.TargetZone == EHitZone::Head) ? 120.f : 80.f;
        const FVector ImpactLoc = Target->GetActorLocation() + ToSelf * 28.f + FVector(0.f, 0.f, ImpactZ);
        const FLinearColor SparkColor = (AttackData.TargetZone == EHitZone::Head)
            ? FLinearColor(1.f, 0.95f, 0.65f) : FLinearColor(1.f, 0.55f, 0.4f);
        SpawnImpactSpark(ImpactLoc, SparkColor, bHeavy ? 1.3f : 0.9f, false);

        if (bHeavy)
        {
            if (ACrowdActor* Crowd = FindCrowd()) Crowd->ReactToHeavyHit();
        }
    }
}

void ABoxerCharacter::SpawnImpactSpark(const FVector& Location, const FLinearColor& Color, float Scale, bool bBigFlash)
{
    if (!ImpactSparkClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    if (AImpactSparkActor* Spark = World->SpawnActor<AImpactSparkActor>(ImpactSparkClass, Location, FRotator::ZeroRotator, Params))
    {
        Spark->Init(Color, Scale, bBigFlash);
    }
}

ACrowdActor* ABoxerCharacter::FindCrowd()
{
    if (CrowdRef.IsValid()) return CrowdRef.Get();
    if (UWorld* World = GetWorld())
    {
        if (ACrowdActor* Crowd = Cast<ACrowdActor>(UGameplayStatics::GetActorOfClass(World, ACrowdActor::StaticClass())))
        {
            CrowdRef = Crowd;
            return Crowd;
        }
    }
    return nullptr;
}

void ABoxerCharacter::OnHitRegistered(EBoxingMove Move)
{
    ComboComponent->RegisterHit(Move);
}

void ABoxerCharacter::OnMissRegistered(EBoxingMove Move)
{
    AudioComponent->PlayMissSwish();
}
