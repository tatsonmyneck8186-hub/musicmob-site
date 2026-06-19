#include "Game/LastBellGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ULastBellGameInstance::StartFight()
{
    UGameplayStatics::OpenLevel(this, FName("L_FightArena"));
}

void ULastBellGameInstance::RecordMatchResult(bool bWon, const TArray<FRoundResult>& Results)
{
    bLastMatchWon = bWon;
    LastMatchResults = Results;

    if (bWon) TotalWins++;
    else TotalLosses++;
}
