#pragma once

#include "CoreMinimal.h"
#include "Core/BoxingTypes.h"
#include "UObject/NoExportTypes.h"
#include "FighterFactory.generated.h"

class UFighterDataAsset;

/**
 * Builds UFighterDataAsset instances at runtime so the game can boot and play
 * a full fight from C++ alone — no hand-authored .uasset Data Assets required.
 * The stats here mirror the values documented in SETUP_GUIDE.md.
 */
UCLASS()
class LASTBELL_API UFighterFactory : public UObject
{
    GENERATED_BODY()

public:
    /** The balanced player fighter. */
    static UFighterDataAsset* MakePlayer(UObject* Outer);

    /** Build an opponent for the given archetype. */
    static UFighterDataAsset* MakeOpponent(UObject* Outer, EFighterType Type);

private:
    static FAttackData MakeAttack(EBoxingMove Move, float Damage, float StaminaCost,
        float KOGain, float Startup, float Active, float Recovery, float Range,
        float HitPause, float Knockback, bool bFlash, EHitZone Zone);
};
