#include "Components/BoxerStatsComponent.h"

UBoxerStatsComponent::UBoxerStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.05f;

    CurrentHealth = 100.f;
    MaxHealth = 100.f;
    CurrentStamina = 100.f;
    MaxStamina = 100.f;
    CurrentKOMeter = 0.f;
    MaxKOMeter = 100.f;
    DefenseMultiplier = 1.f;
    StaminaRegenRate = 15.f;
    StaminaRegenDelay = 1.2f;
    KOMeterDecayRate = 3.f;
    TimeSinceLastStaminaUse = 999.f;
}

void UBoxerStatsComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBoxerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    RegenStamina(DeltaTime);
    DecayKOMeter(DeltaTime);
    TimeSinceLastStaminaUse += DeltaTime;
}

void UBoxerStatsComponent::InitializeStats(const FFighterStats& InStats)
{
    MaxHealth = InStats.MaxHealth;
    CurrentHealth = MaxHealth;
    MaxStamina = InStats.MaxStamina;
    CurrentStamina = MaxStamina;
    MaxKOMeter = InStats.MaxKOMeter;
    CurrentKOMeter = 0.f;
    StaminaRegenRate = InStats.StaminaRegenRate;
    StaminaRegenDelay = InStats.StaminaRegenDelay;
    KOMeterDecayRate = InStats.KOMeterDecayRate;
    DefenseMultiplier = InStats.DefenseMultiplier;
}

void UBoxerStatsComponent::ApplyDamage(float RawDamage)
{
    float ActualDamage = RawDamage * DefenseMultiplier;
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.f, MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        OnKO.Broadcast();
    }
    else if (CurrentKOMeter >= MaxKOMeter)
    {
        OnKnockdown.Broadcast();
        CurrentKOMeter = 0.f;
    }
}

bool UBoxerStatsComponent::ConsumeStamina(float Amount)
{
    if (CurrentStamina < Amount)
    {
        return false;
    }
    CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.f, MaxStamina);
    TimeSinceLastStaminaUse = 0.f;
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    return true;
}

void UBoxerStatsComponent::AddKOMeter(float Amount)
{
    CurrentKOMeter = FMath::Clamp(CurrentKOMeter + Amount, 0.f, MaxKOMeter);
    OnKOMeterChanged.Broadcast(CurrentKOMeter, MaxKOMeter);
}

float UBoxerStatsComponent::GetHealthPercent() const
{
    return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;
}

float UBoxerStatsComponent::GetStaminaPercent() const
{
    return MaxStamina > 0.f ? CurrentStamina / MaxStamina : 0.f;
}

float UBoxerStatsComponent::GetKOMeterPercent() const
{
    return MaxKOMeter > 0.f ? CurrentKOMeter / MaxKOMeter : 0.f;
}

void UBoxerStatsComponent::ResetForNewRound()
{
    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    CurrentKOMeter = 0.f;
    TimeSinceLastStaminaUse = 999.f;

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    OnKOMeterChanged.Broadcast(CurrentKOMeter, MaxKOMeter);
}

void UBoxerStatsComponent::RegenStamina(float DeltaTime)
{
    if (TimeSinceLastStaminaUse < StaminaRegenDelay) return;
    if (CurrentStamina >= MaxStamina) return;

    CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UBoxerStatsComponent::DecayKOMeter(float DeltaTime)
{
    if (CurrentKOMeter <= 0.f) return;
    CurrentKOMeter = FMath::Clamp(CurrentKOMeter - KOMeterDecayRate * DeltaTime, 0.f, MaxKOMeter);
    OnKOMeterChanged.Broadcast(CurrentKOMeter, MaxKOMeter);
}
