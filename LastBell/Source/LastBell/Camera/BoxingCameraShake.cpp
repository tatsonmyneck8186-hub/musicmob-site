#include "Camera/BoxingCameraShake.h"

UBoxingCameraShake_Light::UBoxingCameraShake_Light()
{
    OscillationDuration = 0.18f;
    OscillationBlendInTime = 0.01f;
    OscillationBlendOutTime = 0.05f;

    RotOscillation.Pitch.Amplitude = 0.6f;
    RotOscillation.Pitch.Frequency = 25.f;
    RotOscillation.Roll.Amplitude = 0.3f;
    RotOscillation.Roll.Frequency = 20.f;
}

UBoxingCameraShake_Heavy::UBoxingCameraShake_Heavy()
{
    OscillationDuration = 0.28f;
    OscillationBlendInTime = 0.01f;
    OscillationBlendOutTime = 0.08f;

    RotOscillation.Pitch.Amplitude = 1.2f;
    RotOscillation.Pitch.Frequency = 20.f;
    RotOscillation.Roll.Amplitude = 0.8f;
    RotOscillation.Roll.Frequency = 18.f;

    LocOscillation.X.Amplitude = 2.f;
    LocOscillation.X.Frequency = 15.f;
}

UBoxingCameraShake_KO::UBoxingCameraShake_KO()
{
    OscillationDuration = 0.6f;
    OscillationBlendInTime = 0.02f;
    OscillationBlendOutTime = 0.2f;

    RotOscillation.Pitch.Amplitude = 2.5f;
    RotOscillation.Pitch.Frequency = 15.f;
    RotOscillation.Roll.Amplitude = 2.f;
    RotOscillation.Roll.Frequency = 12.f;
    RotOscillation.Yaw.Amplitude = 1.f;
    RotOscillation.Yaw.Frequency = 10.f;

    LocOscillation.X.Amplitude = 5.f;
    LocOscillation.X.Frequency = 10.f;
    LocOscillation.Y.Amplitude = 3.f;
    LocOscillation.Y.Frequency = 8.f;
}
