#include "Game/FighterFactory.h"
#include "Data/FighterDataAsset.h"

FAttackData UFighterFactory::MakeAttack(EBoxingMove Move, float Damage, float StaminaCost,
    float KOGain, float Startup, float Active, float Recovery, float Range,
    float HitPause, float Knockback, bool bFlash, EHitZone Zone)
{
    FAttackData A;
    A.MoveType = Move;
    A.Damage = Damage;
    A.StaminaCost = StaminaCost;
    A.KOMeterGain = KOGain;
    A.StartupDuration = Startup;
    A.ActiveDuration = Active;
    A.RecoveryDuration = Recovery;
    A.Range = Range;
    A.HitPauseDuration = HitPause;
    A.KnockbackForce = Knockback;
    A.bCausesScreenFlash = bFlash;
    A.ScreenFlashIntensity = bFlash ? 0.35f : 0.f;
    A.TargetZone = Zone;
    return A;
}

UFighterDataAsset* UFighterFactory::MakePlayer(UObject* Outer)
{
    UFighterDataAsset* D = NewObject<UFighterDataAsset>(Outer);
    D->FighterName = FText::FromString(TEXT("You"));
    D->PrimaryColor = FLinearColor(0.1f, 0.45f, 1.f);
    D->SecondaryColor = FLinearColor(0.05f, 0.05f, 0.1f);
    D->FighterType = EFighterType::Player;

    D->Stats.MaxHealth = 100.f;
    D->Stats.MaxStamina = 100.f;
    D->Stats.StaminaRegenRate = 16.f;
    D->Stats.StaminaRegenDelay = 1.1f;
    D->Stats.MaxKOMeter = 100.f;
    D->Stats.KOMeterDecayRate = 3.f;
    D->Stats.DefenseMultiplier = 1.f;
    D->Stats.MovementSpeed = 320.f;

    D->JabData      = MakeAttack(EBoxingMove::Jab,       7.f,  8.f,  6.f,  0.12f, 0.08f, 0.18f, 165.f, 0.05f, 180.f, false, EHitZone::Head);
    D->HookData     = MakeAttack(EBoxingMove::Hook,      13.f, 16.f, 12.f, 0.18f, 0.09f, 0.28f, 150.f, 0.07f, 260.f, false, EHitZone::Head);
    D->UppercutData = MakeAttack(EBoxingMove::Uppercut,  20.f, 26.f, 22.f, 0.24f, 0.10f, 0.40f, 135.f, 0.10f, 400.f, true,  EHitZone::Head);
    return D;
}

UFighterDataAsset* UFighterFactory::MakeOpponent(UObject* Outer, EFighterType Type)
{
    UFighterDataAsset* D = NewObject<UFighterDataAsset>(Outer);
    D->FighterType = Type;

    switch (Type)
    {
    case EFighterType::Counter:
        D->FighterName = FText::FromString(TEXT("Sal \"Slip\" Corrales"));
        D->PrimaryColor = FLinearColor(0.9f, 0.75f, 0.1f);
        D->SecondaryColor = FLinearColor(0.1f, 0.1f, 0.1f);
        D->Stats.MaxHealth = 100.f;
        D->Stats.MaxStamina = 110.f;
        D->Stats.StaminaRegenRate = 18.f;
        D->Stats.StaminaRegenDelay = 1.0f;
        D->Stats.DefenseMultiplier = 1.f;
        D->Stats.MovementSpeed = 340.f;
        D->JabData      = MakeAttack(EBoxingMove::Jab,      8.f,  8.f,  8.f,  0.16f, 0.08f, 0.20f, 165.f, 0.05f, 190.f, false, EHitZone::Head);
        D->HookData     = MakeAttack(EBoxingMove::Hook,     14.f, 15.f, 13.f, 0.20f, 0.08f, 0.26f, 150.f, 0.07f, 270.f, false, EHitZone::Head);
        D->UppercutData = MakeAttack(EBoxingMove::Uppercut, 19.f, 24.f, 20.f, 0.22f, 0.09f, 0.36f, 135.f, 0.10f, 380.f, true,  EHitZone::Head);
        D->Personality.AttackFrequency = 1.6f;
        D->Personality.DodgeProbability = 0.55f;
        D->Personality.CounterProbability = 0.70f;
        D->Personality.MoveWeights = FVector(0.45f, 0.35f, 0.20f);
        D->Personality.FakePunchProbability = 0.25f;
        D->Personality.ComebackAggressionBoost = 0.5f;
        break;

    case EFighterType::Heavyweight:
        D->FighterName = FText::FromString(TEXT("Bruno \"The Boulder\" Mack"));
        D->PrimaryColor = FLinearColor(0.7f, 0.1f, 0.1f);
        D->SecondaryColor = FLinearColor(0.1f, 0.05f, 0.05f);
        D->Stats.MaxHealth = 150.f;
        D->Stats.MaxStamina = 90.f;
        D->Stats.StaminaRegenRate = 12.f;
        D->Stats.StaminaRegenDelay = 1.4f;
        D->Stats.DefenseMultiplier = 0.85f;
        D->Stats.MovementSpeed = 220.f;
        D->JabData      = MakeAttack(EBoxingMove::Jab,      14.f, 12.f, 10.f, 0.22f, 0.10f, 0.26f, 170.f, 0.07f, 260.f, false, EHitZone::Head);
        D->HookData     = MakeAttack(EBoxingMove::Hook,     26.f, 20.f, 18.f, 0.28f, 0.11f, 0.36f, 155.f, 0.10f, 360.f, true,  EHitZone::Head);
        D->UppercutData = MakeAttack(EBoxingMove::Uppercut, 35.f, 30.f, 28.f, 0.34f, 0.12f, 0.50f, 140.f, 0.13f, 520.f, true,  EHitZone::Head);
        D->Personality.AttackFrequency = 0.8f;
        D->Personality.DodgeProbability = 0.15f;
        D->Personality.CounterProbability = 0.2f;
        D->Personality.MoveWeights = FVector(0.30f, 0.30f, 0.40f);
        D->Personality.FakePunchProbability = 0.08f;
        D->Personality.ComebackAggressionBoost = 0.6f;
        break;

    case EFighterType::Rookie:
    default:
        D->FighterName = FText::FromString(TEXT("Rex \"Rookie\" Ramone"));
        D->PrimaryColor = FLinearColor(0.2f, 0.7f, 0.3f);
        D->SecondaryColor = FLinearColor(0.05f, 0.1f, 0.05f);
        D->Stats.MaxHealth = 120.f;
        D->Stats.MaxStamina = 100.f;
        D->Stats.StaminaRegenRate = 14.f;
        D->Stats.StaminaRegenDelay = 1.2f;
        D->Stats.DefenseMultiplier = 1.05f;
        D->Stats.MovementSpeed = 240.f;
        D->JabData      = MakeAttack(EBoxingMove::Jab,      8.f,  9.f,  7.f,  0.22f, 0.09f, 0.24f, 160.f, 0.05f, 180.f, false, EHitZone::Head);
        D->HookData     = MakeAttack(EBoxingMove::Hook,     14.f, 17.f, 12.f, 0.26f, 0.10f, 0.32f, 145.f, 0.07f, 250.f, false, EHitZone::Head);
        D->UppercutData = MakeAttack(EBoxingMove::Uppercut, 19.f, 26.f, 20.f, 0.30f, 0.11f, 0.44f, 130.f, 0.10f, 360.f, true,  EHitZone::Head);
        D->Personality.AttackFrequency = 1.0f;
        D->Personality.DodgeProbability = 0.2f;
        D->Personality.CounterProbability = 0.1f;
        D->Personality.MoveWeights = FVector(0.6f, 0.3f, 0.1f);
        D->Personality.FakePunchProbability = 0.05f;
        D->Personality.ComebackAggressionBoost = 0.3f;
        break;
    }

    return D;
}
