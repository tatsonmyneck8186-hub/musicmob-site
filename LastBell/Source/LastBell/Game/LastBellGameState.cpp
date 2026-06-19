#include "Game/LastBellGameState.h"

void ALastBellGameState::RecordRoundResult(const FRoundResult& Result)
{
    RoundHistory.Add(Result);
}
