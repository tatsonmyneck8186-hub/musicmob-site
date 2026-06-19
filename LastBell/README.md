# LAST BELL — Arcade Boxing Game

A complete Unreal Engine 5.7 arcade boxing game with original characters, C++ gameplay logic, and Blueprint UI.

## Controls

| Key | Action |
|-----|--------|
| A / D | Move left / right |
| S | Duck |
| Space | Dodge |
| Left Mouse Button | Jab |
| Right Mouse Button | Hook |
| E | Uppercut |

## Opponents

| Fighter | Type | Style |
|---------|------|-------|
| Rex "Rookie" Ramone | Rookie | Slow, predictable, forgiving |
| Sal "Slip" Corrales | Counter Boxer | Dodges and counters aggressively |
| Bruno "The Boulder" Mack | Heavyweight | Slow but devastating power |

## Architecture

```
LastBell/
├── Source/LastBell/
│   ├── Core/          — BoxingTypes, IBoxerInterface
│   ├── Data/          — FighterDataAsset, LastBellGameSettings
│   ├── Components/    — StatsComponent, CombatComponent, FeedbackComponent, ComboComponent, AudioComponent, PersonalityComponent
│   ├── Characters/    — BoxerCharacter (abstract), PlayerBoxer, AIBoxer
│   ├── AI/            — BoxingAIController, BTTasks, BTService
│   ├── Game/          — GameMode, GameState, PlayerController, GameInstance
│   ├── UI/            — LastBellHUD, BoxingUserWidget
│   ├── Environment/   — BoxingRingActor, CrowdActor
│   ├── Animation/     — BoxerAnimInstance
│   └── Camera/        — BoxingCameraShake variants
└── Config/            — Engine and Game ini files
```

## Polish Features

- **Three-phase attack system**: Startup (wind-up) → Active (hitbox live) → Recovery (vulnerable)
- **Real-time hit-pause**: `FPlatformTime::Seconds()` based — immune to its own time dilation
- **KO slow-motion**: Smooth ramp-in / hold / ramp-out curve (real-time tracked)
- **Dynamic FOV**: Zoom in on startup, push out on impact
- **Knockback**: `LaunchCharacter` impulse on hit
- **Combo system**: Time-window combo counter with delegates
- **AI personality**: Fatigue accumulation, aggression states, fake punches, comeback mode
- **Layered audio**: Primary impact + flesh layer + heavy layer, stamina breathing loop
- **Instanced crowd**: 120 spectators in one draw call
