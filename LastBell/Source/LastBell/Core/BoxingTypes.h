#pragma once

#include "CoreMinimal.h"
#include "BoxingTypes.generated.h"

UENUM(BlueprintType)
enum class EBoxingMove : uint8
{
    None        UMETA(DisplayName = "None"),
    Jab         UMETA(DisplayName = "Jab"),
    Hook        UMETA(DisplayName = "Hook"),
    Uppercut    UMETA(DisplayName = "Uppercut"),
    Dodge       UMETA(DisplayName = "Dodge")
};

UENUM(BlueprintType)
enum class EHitZone : uint8
{
    Head    UMETA(DisplayName = "Head"),
    Body    UMETA(DisplayName = "Body")
};

UENUM(BlueprintType)
enum class EAttackPhase : uint8
{
    None        UMETA(DisplayName = "None"),
    Startup     UMETA(DisplayName = "Startup"),
    Active      UMETA(DisplayName = "Active"),
    Recovery    UMETA(DisplayName = "Recovery")
};

UENUM(BlueprintType)
enum class EFighterType : uint8
{
    Player      UMETA(DisplayName = "Player"),
    Rookie      UMETA(DisplayName = "Rookie"),
    Counter     UMETA(DisplayName = "Counter Boxer"),
    Heavyweight UMETA(DisplayName = "Heavyweight")
};

UENUM(BlueprintType)
enum class EMatchState : uint8
{
    WaitingToStart  UMETA(DisplayName = "Waiting To Start"),
    RoundActive     UMETA(DisplayName = "Round Active"),
    BetweenRounds   UMETA(DisplayName = "Between Rounds"),
    Knockdown       UMETA(DisplayName = "Knockdown"),
    MatchOver       UMETA(DisplayName = "Match Over")
};

UENUM(BlueprintType)
enum class EBoxerState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Moving      UMETA(DisplayName = "Moving"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Dodging     UMETA(DisplayName = "Dodging"),
    Ducking     UMETA(DisplayName = "Ducking"),
    HitStun     UMETA(DisplayName = "Hit Stun"),
    KnockedDown UMETA(DisplayName = "Knocked Down"),
    KO          UMETA(DisplayName = "KO"),
    Victory     UMETA(DisplayName = "Victory"),
    Defeat      UMETA(DisplayName = "Defeat")
};

UENUM(BlueprintType)
enum class EHitResult : uint8
{
    Miss        UMETA(DisplayName = "Miss"),
    Blocked     UMETA(DisplayName = "Blocked"),
    Hit         UMETA(DisplayName = "Hit"),
    Counter     UMETA(DisplayName = "Counter"),
    KnockDown   UMETA(DisplayName = "Knock Down")
};

UENUM(BlueprintType)
enum class EAggressionState : uint8
{
    Passive     UMETA(DisplayName = "Passive"),
    Normal      UMETA(DisplayName = "Normal"),
    Aggressive  UMETA(DisplayName = "Aggressive"),
    Desperate   UMETA(DisplayName = "Desperate")
};

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBoxingMove MoveType = EBoxingMove::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaCost = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KOMeterGain = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartupDuration = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ActiveDuration = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RecoveryDuration = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HitPauseDuration = 0.06f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KnockbackForce = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KnockbackVerticalRatio = 0.12f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartupFOVDelta = -2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ActiveFOVDelta = 4.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCausesScreenFlash = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ScreenFlashIntensity = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ScreenFlashDuration = 0.08f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EHitZone TargetZone = EHitZone::Head;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USoundBase> SwingSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USoundBase> ImpactSound = nullptr;
};

USTRUCT(BlueprintType)
struct FFighterStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxStamina = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaRegenRate = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaRegenDelay = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxKOMeter = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KOMeterDecayRate = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefenseMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovementSpeed = 300.f;
};

USTRUCT(BlueprintType)
struct FAIPersonality
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackFrequency = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RetreatHealthThreshold = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CounterProbability = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DodgeProbability = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveWeights = FVector(0.5f, 0.3f, 0.2f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FakePunchProbability = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FakePunchCooldown = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FatigueAttackSlowdown = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ComebackHealthThreshold = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ComebackAggressionBoost = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefensivePhaseDuration = 2.f;
};

USTRUCT(BlueprintType)
struct FRoundResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 RoundNumber = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bPlayerWon = false;

    UPROPERTY(BlueprintReadOnly)
    bool bKnockdown = false;

    UPROPERTY(BlueprintReadOnly)
    float TimeElapsed = 0.f;
};
