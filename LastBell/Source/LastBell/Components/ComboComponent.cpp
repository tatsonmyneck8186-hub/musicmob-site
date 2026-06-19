#include "Components/ComboComponent.h"

UComboComponent::UComboComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}

void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CurrentCombo > 0)
    {
        TimeSinceLastHit += DeltaTime;
        if (TimeSinceLastHit >= ComboWindowDuration)
        {
            BreakCombo();
        }
    }
}

void UComboComponent::RegisterHit(EBoxingMove Move)
{
    CurrentCombo++;
    TimeSinceLastHit = 0.f;

    if (CurrentCombo > BestCombo)
    {
        BestCombo = CurrentCombo;
    }

    OnComboUpdated.Broadcast(CurrentCombo, BestCombo);
}

void UComboComponent::BreakCombo()
{
    if (CurrentCombo == 0) return;
    CurrentCombo = 0;
    TimeSinceLastHit = 0.f;
    OnComboReset.Broadcast();
}

void UComboComponent::ResetForNewMatch()
{
    CurrentCombo = 0;
    BestCombo = 0;
    TimeSinceLastHit = 0.f;
}
