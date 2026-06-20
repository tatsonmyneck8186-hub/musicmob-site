#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/BoxingTypes.h"
#include "LastBellGameMode.generated.h"

class ABoxerCharacter;
class UFighterDataAsset;
class UBoxerFeedbackComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, EMatchState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundChanged, int32, CurrentRound, int32, TotalRounds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerTick, float, RemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKnockdownCount, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundEnd, bool, bPlayerWon, bool, bKnockdown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchEnd, bool, bPlayerWon);

UCLASS()
class LASTBELL_API ALastBellGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ALastBellGameMode();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMatchStateChanged OnMatchStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRoundChanged OnRoundChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTimerTick OnTimerTick;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnKnockdownCount OnKnockdownCount;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRoundEnd OnRoundEnd;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMatchEnd OnMatchEnd;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
    int32 TotalRounds = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
    float RoundDuration = 120.f;

    UFUNCTION(BlueprintCallable, Category = "Match")
    void StartMatch(UFighterDataAsset* OpponentData);

    UFUNCTION(BlueprintCallable, Category = "Match")
    void StartRound();

    UFUNCTION(BlueprintCallable, Category = "Match")
    void EndRound(bool bPlayerWon, bool bByKnockdown);

    UFUNCTION(BlueprintCallable, Category = "Match")
    void TriggerKnockdown(ABoxerCharacter* KnockedDownFighter);

    UFUNCTION(BlueprintPure, Category = "Match")
    EMatchState GetMatchState() const { return CurrentMatchState; }

    UFUNCTION(BlueprintPure, Category = "Match")
    int32 GetCurrentRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Match")
    float GetRoundTimeRemaining() const { return RoundTimeRemaining; }

    UFUNCTION(BlueprintPure, Category = "Match")
    int32 GetKnockdownCount() const { return KnockdownCount; }

    UFUNCTION(BlueprintPure, Category = "Match")
    int32 GetTotalRounds() const { return TotalRounds; }

    UFUNCTION(BlueprintPure, Category = "Match")
    bool IsPlayerWinner() const { return bMatchPlayerWon; }

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    TObjectPtr<ABoxerCharacter> PlayerBoxer;

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    TObjectPtr<ABoxerCharacter> AIBoxerRef;

protected:
    virtual void BeginPlay() override;

    /** Presentation hook for a finished match. Base opens the win/lose level; the
     *  arena game mode overrides this to stay in-world (poses + rematch). */
    virtual void HandleMatchEnd(bool bPlayerWon);

    /** Cancel an in-progress match (used when abandoning back to a menu). */
    void AbortMatch();

    bool bMatchPlayerWon = false;

private:
    EMatchState CurrentMatchState = EMatchState::WaitingToStart;
    int32 CurrentRound = 0;
    float RoundTimeRemaining = 0.f;
    int32 PlayerRoundWins = 0;
    int32 AIRoundWins = 0;
    int32 KnockdownCount = 0;

    FTimerHandle RoundTimerHandle;
    FTimerHandle KnockdownTimerHandle;

    void TickRoundTimer();
    void KnockdownCountTick();
    void FinalizeMatch();
    void SetMatchState(EMatchState NewState);
};
