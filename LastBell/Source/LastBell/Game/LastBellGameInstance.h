#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/BoxingTypes.h"
#include "LastBellGameInstance.generated.h"

class UFighterDataAsset;

UCLASS()
class LASTBELL_API ULastBellGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UFighterDataAsset> SelectedOpponent;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    int32 SelectedPlayerSkin = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Results")
    TArray<FRoundResult> LastMatchResults;

    UPROPERTY(BlueprintReadOnly, Category = "Results")
    bool bLastMatchWon = false;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 TotalWins = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 TotalLosses = 0;

    UFUNCTION(BlueprintCallable, Category = "Match")
    void StartFight();

    UFUNCTION(BlueprintCallable, Category = "Match")
    void RecordMatchResult(bool bWon, const TArray<FRoundResult>& Results);
};
