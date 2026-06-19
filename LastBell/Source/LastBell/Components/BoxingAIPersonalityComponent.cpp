#include "Components/BoxingAIPersonalityComponent.h"

UBoxingAIPersonalityComponent::UBoxingAIPersonalityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.2f;
}

void UBoxingAIPersonalityComponent::Initialize(const FAIPersonality& InPersonality)
{
    Personality = InPersonality;
}

void UBoxingAIPersonalityComponent::OnPunchLanded()
{
    FatigueLevel = FMath::Clamp(FatigueLevel + 0.08f, 0.f, 1.f);
}

void UBoxingAIPersonalityComponent::OnPunchReceived(float DamageFraction)
{
    if (DamageFraction > 0.4f)
    {
        FatigueLevel = FMath::Clamp(FatigueLevel + 0.05f, 0.f, 1.f);
    }
}

void UBoxingAIPersonalityComponent::OnHealthUpdated(float HealthPercent)
{
    CurrentHealthPercent = HealthPercent;
    bComebackMode = (HealthPercent <= Personality.ComebackHealthThreshold);
    UpdateAggressionState();
}

void UBoxingAIPersonalityComponent::RecordFakePunch()
{
    TimeSinceFakePunch = 0.f;
}

bool UBoxingAIPersonalityComponent::CanFakePunch() const
{
    return TimeSinceFakePunch >= Personality.FakePunchCooldown &&
        Personality.FakePunchProbability > 0.f;
}

float UBoxingAIPersonalityComponent::GetAttackFrequencyMultiplier() const
{
    float Mult = 1.f;
    if (bComebackMode) Mult += Personality.ComebackAggressionBoost;
    Mult -= FatigueLevel * Personality.FatigueAttackSlowdown;
    return FMath::Clamp(Mult, 0.3f, 2.f);
}

float UBoxingAIPersonalityComponent::GetDodgeProbability() const
{
    float Base = Personality.DodgeProbability;
    if (FatigueLevel > 0.6f) Base *= 0.5f;
    return Base;
}

void UBoxingAIPersonalityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TickFatigue(DeltaTime);
    TimeSinceFakePunch += DeltaTime;
}

void UBoxingAIPersonalityComponent::UpdateAggressionState()
{
    EAggressionState NewState;

    if (bComebackMode)
    {
        NewState = EAggressionState::Desperate;
    }
    else if (CurrentHealthPercent < 0.4f)
    {
        NewState = EAggressionState::Aggressive;
    }
    else if (FatigueLevel > 0.5f)
    {
        NewState = EAggressionState::Passive;
    }
    else
    {
        NewState = EAggressionState::Normal;
    }

    if (NewState != CurrentAggressionState)
    {
        CurrentAggressionState = NewState;
        OnAggressionStateChanged.Broadcast(NewState);
    }
}

void UBoxingAIPersonalityComponent::TickFatigue(float DeltaTime)
{
    if (FatigueLevel > 0.f)
    {
        FatigueLevel = FMath::Clamp(FatigueLevel - 0.02f * DeltaTime, 0.f, 1.f);
    }
}
