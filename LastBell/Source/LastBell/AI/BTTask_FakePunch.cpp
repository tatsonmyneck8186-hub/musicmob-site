#include "AI/BTTask_FakePunch.h"
#include "Characters/AIBoxer.h"
#include "Components/BoxingAIPersonalityComponent.h"
#include "Components/CombatComponent.h"
#include "AIController.h"
#include "Data/FighterDataAsset.h"

UBTTask_FakePunch::UBTTask_FakePunch()
{
    NodeName = "Fake Punch";
}

EBTNodeResult::Type UBTTask_FakePunch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    AAIBoxer* Boxer = Cast<AAIBoxer>(Controller->GetPawn());
    if (!Boxer) return EBTNodeResult::Failed;

    UBoxingAIPersonalityComponent* Personality = Boxer->FindComponentByClass<UBoxingAIPersonalityComponent>();
    if (!Personality || !Personality->CanFakePunch()) return EBTNodeResult::Failed;

    UCombatComponent* Combat = Boxer->FindComponentByClass<UCombatComponent>();
    if (!Combat) return EBTNodeResult::Failed;

    if (Boxer->GetFighterData())
    {
        FAttackData Data = Boxer->GetFighterData()->GetAttackData(FakeMove);
        Combat->TryFakePunch(FakeMove, Data);
        Personality->RecordFakePunch();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
