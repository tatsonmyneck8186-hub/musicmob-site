#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/BoxingTypes.h"
#include "LastBellGameState.generated.h"

UCLASS()
class LASTBELL_API ALastBellGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Match")
    EMatchState CurrentMatchState = EMatchState::WaitingToStart;

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    TArray<FRoundResult> RoundHistory;

    UFUNCTION(BlueprintCallable, Category = "Match")
    void RecordRoundResult(const FRoundResult& Result);
};
