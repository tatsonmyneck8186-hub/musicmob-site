#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/BoxingTypes.h"
#include "FighterDataAsset.generated.h"

UCLASS(BlueprintType)
class LASTBELL_API UFighterDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fighter")
    FText FighterName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fighter")
    TObjectPtr<UTexture2D> Portrait;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fighter")
    FLinearColor PrimaryColor = FLinearColor(1.f, 0.2f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fighter")
    FLinearColor SecondaryColor = FLinearColor(0.1f, 0.1f, 0.1f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    FFighterStats Stats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacks")
    FAttackData JabData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacks")
    FAttackData HookData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacks")
    FAttackData UppercutData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    EFighterType FighterType = EFighterType::Rookie;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    FAIPersonality Personality;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    TObjectPtr<USkeletalMesh> FighterMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
    TSubclassOf<UAnimInstance> AnimClass;

    UFUNCTION(BlueprintCallable, Category = "Fighter")
    FAttackData GetAttackData(EBoxingMove Move) const;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
