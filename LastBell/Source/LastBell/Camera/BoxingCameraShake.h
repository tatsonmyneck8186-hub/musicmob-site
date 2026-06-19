#pragma once

#include "CoreMinimal.h"
#include "LegacyCameraShake.h"
#include "BoxingCameraShake.generated.h"

UCLASS(Blueprintable)
class LASTBELL_API UBoxingCameraShake_Light : public ULegacyCameraShake
{
    GENERATED_BODY()
public:
    UBoxingCameraShake_Light();
};

UCLASS(Blueprintable)
class LASTBELL_API UBoxingCameraShake_Heavy : public ULegacyCameraShake
{
    GENERATED_BODY()
public:
    UBoxingCameraShake_Heavy();
};

UCLASS(Blueprintable)
class LASTBELL_API UBoxingCameraShake_KO : public ULegacyCameraShake
{
    GENERATED_BODY()
public:
    UBoxingCameraShake_KO();
};
