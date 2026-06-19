#include "Characters/PlayerBoxer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxerFeedbackComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerBoxer::APlayerBoxer()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 520.f;
    CameraBoom->SetRelativeRotation(FRotator(-12.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->FieldOfView = 75.f;
    FollowCamera->bUsePawnControlRotation = false;
}

void APlayerBoxer::BeginPlay()
{
    Super::BeginPlay();

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

void APlayerBoxer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AddMovementInput(GetActorRightVector(), HorizontalInput);

    float FOVOffset = FeedbackComponent->GetFOVOffset();
    FollowCamera->FieldOfView = BaseFOV + FOVOffset;

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

void APlayerBoxer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

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
