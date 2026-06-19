#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LastBellGameSettings.generated.h"

class UFighterDataAsset;

UCLASS(BlueprintType)
class LASTBELL_API ULastBellGameSettings : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
    int32 TotalRounds = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match")
    float RoundDuration = 120.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ring")
    float RingBoundsY = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float HitPauseTimeDilationScale = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float HitPauseRealDuration = 0.07f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> BellSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> CrowdCheerSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> BackgroundMusic;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> KOSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fighters")
    TArray<TObjectPtr<UFighterDataAsset>> AvailableOpponents;
};
