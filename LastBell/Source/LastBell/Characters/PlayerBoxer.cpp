#include "Characters/PlayerBoxer.h"
#include "Camera/CameraComponent.h"
#include "Camera/BoxingCameraShake.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputModifiers.h"
#include "Components/BoxerFeedbackComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerBoxer::APlayerBoxer()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 60.f));   // aim at chest height
    CameraBoom->TargetArmLength = 620.f;
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;
    // Keep a consistent side-on arcade view regardless of which way the boxer faces.
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->SetWorldRotation(FRotator(-14.f, 70.f, 0.f));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->FieldOfView = 78.f;
    FollowCamera->bUsePawnControlRotation = false;

    ShakeLight = UBoxingCameraShake_Light::StaticClass();
    ShakeHeavy = UBoxingCameraShake_Heavy::StaticClass();
    ShakeKO = UBoxingCameraShake_KO::StaticClass();
}

void APlayerBoxer::BeginPlay()
{
    Super::BeginPlay();

    EnsureRuntimeInput();

    if (CombatComponent)
    {
        CombatComponent->OnHitLanded.AddDynamic(this, &APlayerBoxer::OnPlayerLandedHit);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void APlayerBoxer::EnsureRuntimeInput()
{
    if (IA_Jab)
    {
        return;
    }

    auto MakeAction = [this](EInputActionValueType Type)
    {
        UInputAction* Action = NewObject<UInputAction>(this);
        Action->ValueType = Type;
        return Action;
    };

    IA_MoveHorizontal = MakeAction(EInputActionValueType::Axis1D);
    IA_Jab            = MakeAction(EInputActionValueType::Boolean);
    IA_Hook           = MakeAction(EInputActionValueType::Boolean);
    IA_Uppercut       = MakeAction(EInputActionValueType::Boolean);
    IA_Dodge          = MakeAction(EInputActionValueType::Boolean);
    IA_Duck           = MakeAction(EInputActionValueType::Boolean);

    UInputMappingContext* IMC = NewObject<UInputMappingContext>(this);
    IMC->MapKey(IA_Jab, EKeys::LeftMouseButton);
    IMC->MapKey(IA_Hook, EKeys::RightMouseButton);
    IMC->MapKey(IA_Uppercut, EKeys::E);
    IMC->MapKey(IA_Dodge, EKeys::SpaceBar);
    IMC->MapKey(IA_Duck, EKeys::S);

    IMC->MapKey(IA_MoveHorizontal, EKeys::D);
    FEnhancedActionKeyMapping& LeftMap = IMC->MapKey(IA_MoveHorizontal, EKeys::A);
    LeftMap.Modifiers.Add(NewObject<UInputModifierNegate>(this));

    DefaultMappingContext = IMC;
}

void APlayerBoxer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AddMovementInput(GetActorRightVector(), HorizontalInput);

    UpdateArcadeCamera(DeltaTime);

    // Drive the per-phase FOV punch through the feedback component.
    EAttackPhase Phase = CombatComponent->GetCurrentAttackPhase();
    if (Phase == EAttackPhase::Startup)
    {
        FeedbackComponent->SetFOVOffset(-2.f);
    }
    else if (Phase == EAttackPhase::Active)
    {
        FeedbackComponent->SetFOVOffset(4.f);
    }
}

void APlayerBoxer::UpdateArcadeCamera(float DeltaTime)
{
    SwayTime += DeltaTime;

    // Pull the camera back as the fighters separate so both stay framed.
    float Sep = 300.f;
    if (AActor* Opp = OpponentRef.Get())
    {
        Sep = FVector::Dist2D(GetActorLocation(), Opp->GetActorLocation());
    }
    const float DesiredArm = FMath::Clamp(520.f + Sep * 0.55f, 520.f, 900.f);
    CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DesiredArm, DeltaTime, 3.f);

    // Gentle idle sway.
    const float SwayYaw = FMath::Sin(SwayTime * 0.6f) * 1.5f;
    const float SwayPitch = FMath::Sin(SwayTime * 0.45f) * 0.8f;
    CameraBoom->SetWorldRotation(FRotator(-14.f + SwayPitch, 70.f + SwayYaw, 0.f));

    // FOV: base + per-phase feedback offset + decaying impact pulse.
    ImpactZoom = FMath::FInterpTo(ImpactZoom, 0.f, DeltaTime, 6.f);
    const float FOVOffset = FeedbackComponent->GetFOVOffset();
    FollowCamera->FieldOfView = BaseFOV + FOVOffset + ImpactZoom;
}

void APlayerBoxer::PlayImpactShake(bool bHeavy)
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        TSubclassOf<UCameraShakeBase> Shake = bHeavy ? ShakeHeavy : ShakeLight;
        if (Shake)
        {
            PC->ClientStartCameraShake(Shake, 1.f);
        }
    }
    ImpactZoom = bHeavy ? -7.f : -3.f;
}

void APlayerBoxer::OnPlayerLandedHit(AActor* Target, FAttackData AttackData)
{
    const bool bHeavy = AttackData.bCausesScreenFlash || AttackData.Damage >= 15.f;
    PlayImpactShake(bHeavy);
}

void APlayerBoxer::ReceiveHit_Implementation(FAttackData AttackData, AActor* Attacker)
{
    Super::ReceiveHit_Implementation(AttackData, Attacker);
    PlayImpactShake(AttackData.Damage >= 18.f);
}

void APlayerBoxer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    EnsureRuntimeInput();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(IA_MoveHorizontal, ETriggerEvent::Triggered, this, &APlayerBoxer::OnMoveHorizontal);
        EIC->BindAction(IA_MoveHorizontal, ETriggerEvent::Completed, this, &APlayerBoxer::OnMoveHorizontal);
        EIC->BindAction(IA_Jab, ETriggerEvent::Triggered, this, &APlayerBoxer::OnJab);
        EIC->BindAction(IA_Hook, ETriggerEvent::Triggered, this, &APlayerBoxer::OnHook);
        EIC->BindAction(IA_Uppercut, ETriggerEvent::Triggered, this, &APlayerBoxer::OnUppercut);
        EIC->BindAction(IA_Dodge, ETriggerEvent::Triggered, this, &APlayerBoxer::OnDodge);
        EIC->BindAction(IA_Duck, ETriggerEvent::Triggered, this, &APlayerBoxer::OnDuckPressed);
        EIC->BindAction(IA_Duck, ETriggerEvent::Completed, this, &APlayerBoxer::OnDuckReleased);
    }
}

void APlayerBoxer::OnMoveHorizontal(const FInputActionValue& Value)
{
    HorizontalInput = Value.Get<float>();
}

void APlayerBoxer::OnJab(const FInputActionValue& Value)
{
    ExecuteAttack_Implementation(EBoxingMove::Jab);
}

void APlayerBoxer::OnHook(const FInputActionValue& Value)
{
    ExecuteAttack_Implementation(EBoxingMove::Hook);
}

void APlayerBoxer::OnUppercut(const FInputActionValue& Value)
{
    ExecuteAttack_Implementation(EBoxingMove::Uppercut);
}

void APlayerBoxer::OnDodge(const FInputActionValue& Value)
{
    ExecuteDodge_Implementation();
}

void APlayerBoxer::OnDuckPressed(const FInputActionValue& Value)
{
    ExecuteDuck_Implementation();
}

void APlayerBoxer::OnDuckReleased(const FInputActionValue& Value)
{
    StopDuck_Implementation();
}
