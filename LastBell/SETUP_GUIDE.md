# LAST BELL — Unreal Editor Setup Guide

This guide walks through every manual step required inside Unreal Editor to get LAST BELL running from source. Follow sections in order — later sections depend on assets created in earlier ones.

---

## 1. Project Setup

**Open the project in UE 5.7**

1. Launch the **Epic Games Launcher**, make sure UE 5.7 is installed under the **Library** tab.
2. Click **Browse** in the launcher (or double-click `LastBell.uproject` in File Explorer).
3. UE will prompt to rebuild missing modules — click **Yes** and wait for the C++ compile to finish. If the compile fails, open Visual Studio and build the `LastBell` target manually (`Build > Build LastBell`).

**Enable required plugins** (Edit > Plugins):

| Plugin | Why |
|--------|-----|
| Enhanced Input | Input Actions and Input Mapping Contexts |
| Behavior Tree | AI decision logic |
| AI Module | AIController, perception, navigation |
| Gameplay Abilities (optional) | If you extend to GAS later |
| Instanced Static Meshes | Efficient crowd rendering |

After enabling plugins, restart the editor when prompted.

**Project settings to check** (Edit > Project Settings):

- **Maps & Modes > Default GameMode**: leave blank for now (set per-map in section 12).
- **Input > Default Input Component Class**: `EnhancedInputComponent`.
- **Rendering > Default Feature Level**: SM6 (required for Nanite/Lumen if used).
- **General Settings > Default Maps**: set Editor Startup Map to `L_FightArena` once maps exist.

---

## 2. Folder Structure

Create the following folder hierarchy inside the **Content Browser**. Right-click in the Content Browser root and choose **New Folder** for each:

```
Content/
└── LastBell/
    ├── Blueprints/
    │   ├── Characters/
    │   ├── AI/
    │   ├── Game/
    │   └── Environment/
    ├── Data/
    ├── Maps/
    ├── Materials/
    ├── Meshes/
    ├── Sounds/
    ├── UI/
    │   └── Widgets/
    └── Input/
```

Keeping assets under `Content/LastBell/` prevents name collisions if you later merge plugin content or marketplace packs.

---

## 3. Input Actions

All input is driven by **Enhanced Input**. You need one **Input Mapping Context** and six **Input Actions**.

### Create Input Actions

In `Content/LastBell/Input/`, right-click > **Input > Input Action** for each:

| Asset Name | Value Type | Notes |
|------------|------------|-------|
| `IA_MoveHorizontal` | `Axis1D (float)` | -1 = left, +1 = right |
| `IA_Jab` | `Digital (bool)` | Triggered on press |
| `IA_Hook` | `Digital (bool)` | Triggered on press |
| `IA_Uppercut` | `Digital (bool)` | Triggered on press |
| `IA_Dodge` | `Digital (bool)` | Triggered on press |
| `IA_Duck` | `Digital (bool)` | Triggered on press and hold |

For `IA_Duck`, set **Trigger** to `Down` so the duck state persists while S is held.

### Create Input Mapping Context

Right-click > **Input > Input Mapping Context**, name it `IMC_Boxing`. Open it and add bindings:

| Input Action | Key | Modifiers |
|-------------|-----|-----------|
| `IA_MoveHorizontal` | `A` | **Negate** (so A = -1) |
| `IA_MoveHorizontal` | `D` | *(none — D = +1)* |
| `IA_Jab` | `Left Mouse Button` | *(none)* |
| `IA_Hook` | `Right Mouse Button` | *(none)* |
| `IA_Uppercut` | `E` | *(none)* |
| `IA_Dodge` | `Space` | *(none)* |
| `IA_Duck` | `S` | *(none)* |

Save `IMC_Boxing`.

---

## 4. Fighter Data Assets

Data Assets drive all fighter statistics. The C++ class `UFighterDataAsset` (in `Source/LastBell/Data/`) is the parent.

In `Content/LastBell/Data/`, right-click > **Miscellaneous > Data Asset**, choose `FighterDataAsset` as the class.

### DA_PlayerFighter

| Property | Value |
|----------|-------|
| Fighter Name | `Player` |
| Max Health | `120` |
| Max Stamina | `110` |
| Jab Startup (ms) | `180` |
| Hook Startup (ms) | `220` |
| Uppercut Startup (ms) | `260` |
| Jab Damage | `8` |
| Hook Damage | `14` |
| Uppercut Damage | `20` |
| Move Speed | `400` |
| Dodge Duration (s) | `0.25` |

### DA_Rookie (Rex "Rookie" Ramone)

| Property | Value |
|----------|-------|
| Fighter Name | `Rex Rookie Ramone` |
| Max Health | `120` |
| Max Stamina | `100` |
| Jab Startup (ms) | `220` |
| Hook Startup (ms) | `280` |
| Uppercut Startup (ms) | `340` |
| Jab Damage | `7` |
| Hook Damage | `12` |
| Uppercut Damage | `18` |
| Dodge Probability | `0.15` |
| Counter Probability | `0.10` |
| AI Behavior Tree | *(assign BT_Rookie after section 7)* |
| Notes | Slow startup, low dodge — good for learning the game |

### DA_Counter (Sal "Slip" Corrales)

| Property | Value |
|----------|-------|
| Fighter Name | `Sal Slip Corrales` |
| Max Health | `100` |
| Max Stamina | `110` |
| Jab Startup (ms) | `160` |
| Hook Startup (ms) | `200` |
| Uppercut Startup (ms) | `240` |
| Jab Damage | `8` |
| Hook Damage | `13` |
| Uppercut Damage | `19` |
| Dodge Probability | `0.55` |
| Counter Probability | `0.70` |
| AI Behavior Tree | *(assign BT_Counter after section 7)* |
| Notes | Fast attacks, high dodge/counter — punishes careless swings |

### DA_Heavyweight (Bruno "The Boulder" Mack)

| Property | Value |
|----------|-------|
| Fighter Name | `Bruno The Boulder Mack` |
| Max Health | `150` |
| Max Stamina | `90` |
| Jab Startup (ms) | `280` |
| Hook Startup (ms) | `340` |
| Uppercut Startup (ms) | `400` |
| Jab Damage | `20` |
| Hook Damage | `28` |
| Uppercut Damage | `35` |
| Dodge Probability | `0.08` |
| Counter Probability | `0.20` |
| Move Speed | `260` |
| AI Behavior Tree | *(assign BT_Heavyweight after section 7)* |
| Notes | Every punch at max damage is fight-ending — slow but lethal |

---

## 5. Game Settings Asset

Right-click in `Content/LastBell/Data/` > **Miscellaneous > Data Asset**, choose `ULastBellGameSettings`.

Name it `DA_GameSettings`. Set these properties:

| Property | Value |
|----------|-------|
| Number of Rounds | `3` |
| Round Duration (s) | `120` |
| Knockdown Count Before KO | `3` |
| Ring Bounds Radius (cm) | `400` |
| KO Slow-Mo Scale | `0.25` |
| KO Slow-Mo Ramp In (s) | `0.15` |
| KO Slow-Mo Hold (s) | `1.5` |
| KO Slow-Mo Ramp Out (s) | `0.4` |
| Hit Pause Duration (s) | `0.06` |
| Combo Window (s) | `1.2` |

Assign `DA_GameSettings` in the `BP_LastBellGameMode` Blueprint (section 9) so the game reads it at runtime.

---

## 6. Blueprint Characters

### BP_PlayerBoxer

1. In `Content/LastBell/Blueprints/Characters/`, right-click > **Blueprint Class**.
2. Search for and select `APlayerBoxer` (your C++ class) as the parent.
3. Name it `BP_PlayerBoxer`.
4. Open `BP_PlayerBoxer` and configure the **Defaults** panel:
   - **Fighter Data**: assign `DA_PlayerFighter`.
   - **Skeletal Mesh**: assign your boxer mesh asset (placeholder or final).
   - **Anim Class**: assign `ABP_Boxer` (created in section 14).
   - **Capsule Half Height** / **Radius**: match your mesh proportions (~90 / 30 cm).
5. In the **Components** panel, select the **Enhanced Input** section:
   - **Default Mapping Context**: assign `IMC_Boxing`.
   - Confirm all six Input Actions are referenced in the Blueprint's `BeginPlay` or in C++ `SetupPlayerInputComponent`.
6. Compile and save.

### BP_AIBoxer

1. Right-click > **Blueprint Class**, parent = `AAIBoxer`.
2. Name it `BP_AIBoxer`.
3. Open defaults:
   - **Fighter Data**: leave blank — the Game Mode or selector screen will assign it at spawn time based on the chosen opponent.
   - **Skeletal Mesh** and **Anim Class**: same assets as BP_PlayerBoxer (swap meshes/tints per fighter in a child Blueprint if desired).
   - **AI Controller Class**: assign `BP_BoxingAIController` (created in section 8).
4. Compile and save.

---

## 7. Behavior Trees

Create Behavior Trees in `Content/LastBell/AI/`. Right-click > **Artificial Intelligence > Behavior Tree** for each, and a matching **Blackboard** (right-click > **Artificial Intelligence > Blackboard**).

### Shared Blackboard Keys (BB_Boxer)

Create one Blackboard `BB_Boxer` and add these keys:

| Key Name | Type | Notes |
|----------|------|-------|
| `PlayerRef` | Object (ABoxerCharacter) | Reference to the player |
| `DistanceToPlayer` | Float | Updated each tick by BT Service |
| `bIsPlayerAttacking` | Bool | True when player is in startup/active phase |
| `bShouldDodge` | Bool | Set by personality logic |
| `bShouldCounter` | Bool | Set after successful dodge |
| `AggressionLevel` | Float | 0.0–1.0, drives attack frequency |
| `FatigueLevel` | Float | 0.0–1.0, reduces action rate |

### BT_Rookie

Simple linear tree — approach then attack, no dodge logic:

```
Root
└── Selector
    ├── Sequence [IsInRange: 150cm]
    │   ├── BTTask_ChooseAttack (weights: Jab 60%, Hook 30%, Uppercut 10%)
    │   └── BTTask_ExecuteAttack
    └── BTTask_MoveToPlayer (AcceptanceRadius: 150)
```

- **BTTask_MoveToPlayer**: Use the built-in `Move To` task pointing at `PlayerRef`.
- **BTTask_ChooseAttack**: Custom task that writes the chosen attack type to the Blackboard.
- **BTTask_ExecuteAttack**: Custom task that calls `CombatComponent->BeginAttack(AttackType)` on the AIBoxer.
- Add a **Wait** node (0.5–1.0 s) after each attack to simulate slow reaction.

### BT_Counter

Dodge-heavy tree with counter window:

```
Root
└── Selector
    ├── Sequence [bShouldCounter == true]
    │   ├── BTTask_ExecuteAttack (forced Hook)
    │   └── BTTask_SetBlackboardBool (bShouldCounter = false)
    ├── Sequence [bIsPlayerAttacking == true]
    │   ├── BTTask_Dodge
    │   └── BTTask_SetBlackboardBool (bShouldCounter = true)
    ├── Sequence [IsInRange: 130cm]
    │   ├── BTTask_ChooseAttack (Jab 70%, Hook 20%, Uppercut 10%)
    │   └── BTTask_ExecuteAttack
    └── BTTask_MoveToPlayer (AcceptanceRadius: 130)
```

- **BTTask_Dodge**: Calls `CombatComponent->BeginDodge()` on the AIBoxer.
- The `bIsPlayerAttacking` key is written by **BTService_ObservePlayer** (see below).

### BT_Heavyweight

Slow advance with devastating punishment:

```
Root
└── Selector
    ├── Sequence [IsInRange: 170cm AND FatigueLevel < 0.6]
    │   ├── BTTask_ChooseAttack (Jab 20%, Hook 40%, Uppercut 40%)
    │   └── BTTask_ExecuteAttack
    ├── Sequence [FatigueLevel >= 0.6]
    │   └── BTTask_Rest (Wait 1.5s, reduces FatigueLevel)
    └── BTTask_MoveToPlayer (AcceptanceRadius: 170, MaxSpeed: 260)
```

- Heavy attacks cost more stamina — `FatigueLevel` rises with each punch and drops during rest.
- Dodge probability is near zero; the heavyweight simply tanks hits and advances.

### BTService_ObservePlayer

Create one shared BT Service (`Content/LastBell/AI/BTService_ObservePlayer`):

- Runs every **0.1 s** (Interval).
- Gets the player character, reads its `CombatComponent->GetCurrentAttackPhase()`.
- Writes `bIsPlayerAttacking = (Phase == EAttackPhase::Startup || Phase == EAttackPhase::Active)`.
- Updates `DistanceToPlayer` with the current distance.
- Updates `AggressionLevel` and `FatigueLevel` from `PersonalityComponent`.

Attach this service to the **Root** node of all three Behavior Trees so it always runs.

---

## 8. AI Controller Blueprint

1. In `Content/LastBell/Blueprints/AI/`, right-click > **Blueprint Class**, parent = `ABoxingAIController`.
2. Name it `BP_BoxingAIController`.
3. Open the Blueprint. In **Event BeginPlay** (or override `OnPossess`):
   - Get the possessed pawn, cast to `AAIBoxer`.
   - Get `FighterData` from the AIBoxer.
   - Branch on `FighterData->FighterType` (Rookie / Counter / Heavyweight).
   - Call `RunBehaviorTree(BT_Rookie)`, `RunBehaviorTree(BT_Counter)`, or `RunBehaviorTree(BT_Heavyweight)` accordingly.
4. Set **Blackboard Asset** to `BB_Boxer` in the Controller defaults.
5. Compile and save.

---

## 9. Game Mode Blueprint

1. In `Content/LastBell/Blueprints/Game/`, right-click > **Blueprint Class**, parent = `ALastBellGameMode`.
2. Name it `BP_LastBellGameMode`.
3. Open defaults and assign:
   - **Default Pawn Class**: `BP_PlayerBoxer`
   - **Player Controller Class**: create `BP_LastBellPlayerController` (parent = `ALastBellPlayerController`) and assign it here.
   - **HUD Class**: `BP_LastBellHUD` (created in section 10).
   - **Game State Class**: `ALastBellGameState` (C++ class, no Blueprint needed unless you add custom logic).
   - **Game Settings**: assign `DA_GameSettings`.
4. In `BP_LastBellGameMode > Event BeginPlay`:
   - Call `SpawnAIBoxer` (your C++ function or a Blueprint implementation) at the AI spawn point in the level.
   - Pass the selected `FighterDataAsset` from `GameInstance` (stored there by the character select screen).
5. Compile and save.

---

## 10. HUD Blueprint

1. In `Content/LastBell/Blueprints/Game/`, right-click > **Blueprint Class**, parent = `ALastBellHUD`.
2. Name it `BP_LastBellHUD`.
3. Open the Blueprint. Add variables for each widget reference:
   - `HUDWidget` — type `WBP_HUD` (created in section 11).
   - `KnockdownWidget` — type `WBP_KnockdownCount`.
4. In **Event BeginPlay**:
   - `Create Widget` (class = `WBP_HUD`) and store in `HUDWidget`.
   - `Add to Viewport` with ZOrder 0.
5. Expose functions the GameMode calls:
   - `ShowKnockdownCount(int Count)` — creates/updates `WBP_KnockdownCount`.
   - `HideKnockdownCount()`.
   - `ShowWinScreen()` / `ShowLoseScreen()`.
6. Compile and save.

---

## 11. UMG Widgets

Create all widgets in `Content/LastBell/UI/Widgets/`. Right-click > **User Interface > Widget Blueprint** for each.

### WBP_HUD

The main in-fight overlay. Layout in the **Designer** tab:

- **Player health bar** (top-left): `UProgressBar`, bind `Percent` to `PlayerBoxer->StatsComponent->GetHealthPercent()`.
- **Player stamina bar** (below health): `UProgressBar`, bind to `GetStaminaPercent()`.
- **AI health bar** (top-right, mirrored): same binding from AIBoxer.
- **AI stamina bar**: same.
- **Round timer** (top-center): `UTextBlock`, bind to `GameState->GetRemainingTimeFormatted()` (returns `"1:47"` style string).
- **Round indicator** (above timer): `UTextBlock`, e.g., `"Round 2 / 3"`.
- **Combo counter** (bottom-center): `UTextBlock` + `UImage` for combo flash. Bind to `ComboComponent->GetCurrentCombo()`. Hide when combo < 2.

Tick the **Is Variable** checkbox on each widget element so they can be referenced in the Graph.

In the **Graph** tab, implement:
- `UpdateHealthBars()` — called by HUD Blueprint each frame or via delegates.
- `FlashCombo(int Count)` — plays a brief scale animation on the combo text.

### WBP_MainMenu

Simple vertical stack center-screen:

- **Title text**: `UTextBlock` — "LAST BELL", large font.
- **Play button**: `UButton` with `UTextBlock` "PLAY". On **Clicked**: `Open Level (L_CharacterSelect)`.
- **Quit button**: `UButton` "QUIT". On **Clicked**: `Quit Game`.

### WBP_CharSelect

Three opponent panels side by side:

- Each panel: `UButton` containing a `UVerticalBox` with fighter portrait (`UImage`), name (`UTextBlock`), and a brief stats summary.
- On **Clicked** for each panel:
  1. Get `GameInstance`, cast to `ULastBellGameInstance`.
  2. Call `SetSelectedFighter(DA_Rookie / DA_Counter / DA_Heavyweight)`.
  3. `Open Level (L_FightArena)`.

### WBP_KnockdownCount

Displayed on knockdown, center-screen:

- Large `UTextBlock` for the countdown number (3 → 2 → 1).
- Animate with a scale pulse using a `UWidgetAnimation` played each second.
- HUD Blueprint controls visibility and count value.

### WBP_WinScreen

- "YOU WIN!" text (large, gold color).
- Final stats: rounds won, total punches landed, accuracy.
- **Play Again** button: `Open Level (L_CharacterSelect)`.
- **Main Menu** button: `Open Level (L_MainMenu)`.

### WBP_LoseScreen

- "K.O.!" text (large, red).
- Same stat layout and buttons as WBP_WinScreen.
- **Rematch** button: `Open Level (L_FightArena)` with the same `GameInstance` fighter still selected.

---

## 12. Maps

Create all maps in `Content/LastBell/Maps/`. Right-click in Content Browser > **Level** (or File > New Level > Empty Level).

### L_MainMenu

1. Create an empty level with no gameplay actors.
2. Add a **Level Blueprint** (`Blueprints` button in toolbar > **Open Level Blueprint**).
3. In **Event BeginPlay**:
   - `Create Widget (WBP_MainMenu)` > `Add to Viewport`.
   - `Set Input Mode UI Only` on the Player Controller.
   - `Show Mouse Cursor = true`.
4. Set sky/background: add a simple **Sky Atmosphere** + **Directional Light** or a fullscreen background image.

### L_CharacterSelect

1. Empty level with background environment (optional: a stylized gym or arena lobby).
2. Level Blueprint **Event BeginPlay**:
   - `Create Widget (WBP_CharSelect)` > `Add to Viewport`.
   - `Set Input Mode UI Only`.
   - `Show Mouse Cursor = true`.

### L_FightArena

This is the main gameplay level. Place the following actors:

| Actor | Class | Placement |
|-------|-------|-----------|
| Boxing Ring | `BP_BoxingRingActor` | Center of level (0, 0, 0) |
| Crowd | `BP_CrowdActor` | Surrounding the ring, ~600 cm radius |
| Player Spawn | `PlayerStart` | Ring corner 1 (e.g., -200, 0, 0) |
| AI Spawn | `TargetPoint` (or custom) | Ring corner 2 (200, 0, 0), tag = `"AISpawn"` |
| Directional Light | — | Overhead, warm tungsten color |
| Sky Light | — | Low intensity fill |
| Post Process Volume | — | Unbounded, slight vignette + contrast |
| `ALastBellGameMode` | — | Set via World Settings > Game Mode Override: `BP_LastBellGameMode` |

In **World Settings**:
- **Game Mode Override**: `BP_LastBellGameMode`.
- **Default Pawn Class**: `BP_PlayerBoxer` (inherited from game mode, but verify here).

In the Level Blueprint **Event BeginPlay**:
- `Set Input Mode Game Only`.
- `Show Mouse Cursor = false`.

### L_WinScreen

1. Empty level.
2. Level Blueprint spawns `WBP_WinScreen` and sets UI-only input mode.
3. Game Mode calls `Open Level (L_WinScreen)` after detecting win condition.

### L_LoseScreen

1. Empty level.
2. Level Blueprint spawns `WBP_LoseScreen` and sets UI-only input mode.
3. Game Mode calls `Open Level (L_LoseScreen)` after KO or time-out loss.

---

## 13. Environment Blueprints

### BP_BoxingRingActor

1. In `Content/LastBell/Blueprints/Environment/`, right-click > **Blueprint Class**, parent = `ABoxingRingActor`.
2. Name it `BP_BoxingRingActor`.
3. In the **Components** panel, add:
   - `StaticMeshComponent` (ring canvas/floor mesh) — assign your ring mesh from `Content/LastBell/Meshes/`.
   - `StaticMeshComponent` (ropes) — or use a spline mesh for ropes.
   - `BoxComponent` x4 (rope collision boundaries) — tag each `"RopeBoundary"`.
4. The C++ parent `ABoxingRingActor` uses `RingBoundsRadius` from `DA_GameSettings`. Verify the Blueprint's bounds match (400 cm radius from center by default).
5. Optionally add a **PointLight** or **SpotLight** overhead to simulate ring lights.

### BP_CrowdActor

1. Right-click > **Blueprint Class**, parent = `ACrowdActor`.
2. Name it `BP_CrowdActor`.
3. In **Components**, add an `InstancedStaticMeshComponent` (ISMC):
   - **Static Mesh**: assign a crowd spectator mesh (a simple seated human silhouette or stylized low-poly model from `Content/LastBell/Meshes/`).
   - In C++ (`ACrowdActor::BeginPlay`), 120 instances are added in a circle. Verify this works by placing `BP_CrowdActor` in the level and checking the instance count in the Details panel.
4. The crowd reacts to game events via the `OnCrowdReact` delegate — no Blueprint wiring needed unless you want to add material parameter changes (e.g., crowd color flash on KO).

---

## 14. Animation Blueprint

Create the Animation Blueprint in `Content/LastBell/Blueprints/` (or a dedicated `Content/LastBell/Animation/` folder).

1. Right-click > **Animation > Animation Blueprint**.
2. **Skeleton**: select the skeleton used by your boxer mesh.
3. Name it `ABP_Boxer`.

### State Machine Setup

Open `ABP_Boxer` and create a **State Machine** called `BoxingLocomotion` in the **AnimGraph**.

Add the following states:

| State | Animation Sequence | Entry Condition |
|-------|-------------------|-----------------|
| `Idle` | `Anim_Idle` | Default state |
| `Move` | `Anim_Run` (blendspace) | `Speed > 10` |
| `Jab` | `Anim_Jab` | `bPlayJab == true` |
| `Hook` | `Anim_Hook` | `bPlayHook == true` |
| `Uppercut` | `Anim_Uppercut` | `bPlayUppercut == true` |
| `Duck` | `Anim_Duck` | `bIsDucking == true` |
| `Dodge` | `Anim_Dodge` | `bIsDodging == true` |
| `HitStun` | `Anim_HitStun` | `bIsHitStunned == true` |
| `KnockDown` | `Anim_KnockDown` | `bIsKnockedDown == true` |
| `KO` | `Anim_KO` | `bIsKO == true` |
| `Victory` | `Anim_Victory` | `bIsVictory == true` |
| `Defeat` | `Anim_Defeat` | `bIsDefeat == true` |

**Transitions** to note:
- Any attack state (`Jab/Hook/Uppercut`) → `Idle`: transition when the animation finishes (`Time Remaining < 0.1s` or on `AnimNotify_AttackEnd`).
- `Idle/Move` → attack states: on bool flag (set by C++ `CombatComponent`).
- `KnockDown` → `Idle`: after knockdown recovery animation finishes.
- `KO` is a terminal state — no exit.

### AnimGraph Wiring

```
[State Machine: BoxingLocomotion] → [Output Pose]
```

For layered additive hits, you can add a `LayeredBlendPerBone` node between the state machine and Output Pose to layer upper-body hit reactions onto locomotion.

### Event Graph Variables

In the **EventGraph**, use `BlueprintUpdateAnimation` to pull state from the owning pawn:

```
Event BlueprintUpdateAnimation
→ Try Get Pawn Owner
→ Cast to ABoxerCharacter
→ Get CombatComponent
→ Read bIsAttacking, CurrentAttackType, bIsDucking, bIsDodging, etc.
→ Set local bool variables (bPlayJab, bPlayHook, etc.)
→ Get CharacterMovement > Velocity > VectorLength → Speed
```

All animation sequences (`Anim_Idle`, `Anim_Jab`, etc.) must be imported into `Content/LastBell/Meshes/` and assigned inside each state node.

---

## 15. Audio Setup

### Sound Asset Organization

Place all audio assets in `Content/LastBell/Sounds/`. Import WAV files via drag-and-drop into the Content Browser (UE auto-creates `USoundWave` assets).

Suggested folder layout:
```
Sounds/
├── Impacts/
│   ├── SW_ImpactPrimary.wav     — clean punch impact
│   ├── SW_ImpactFlesh.wav       — flesh layer (wet, muffled)
│   └── SW_ImpactHeavy.wav       — heavyweight bonus layer
├── Ambience/
│   ├── SW_BreathingLoop.wav     — looping stamina breath (increase pitch at low stamina)
│   └── SW_CrowdAmbience.wav     — background crowd noise loop
├── Events/
│   ├── SW_Bell.wav              — round start/end bell
│   ├── SW_KO.wav                — KO announcement sting
│   └── SW_ComboHit.wav          — combo tick sound
└── UI/
    ├── SW_ButtonClick.wav
    └── SW_MenuMusic.wav         — loop for main menu
```

### Assigning Sounds in BoxerAudioComponent

The C++ `UBoxerAudioComponent` has `USoundBase*` properties for each slot. Assign them in `BP_PlayerBoxer` and `BP_AIBoxer`:

1. Select `BP_PlayerBoxer` in the Content Browser, open it.
2. In the **Components** panel, click `BoxerAudioComponent`.
3. In the **Details** panel, find the audio slots and assign:
   - **Impact Sound Primary**: `SW_ImpactPrimary`
   - **Impact Sound Flesh**: `SW_ImpactFlesh`
   - **Impact Sound Heavy**: `SW_ImpactHeavy` (heavyweight only — assign in a child BP or set conditionally)
   - **Breathing Loop**: `SW_BreathingLoop`
   - **Combo Sound**: `SW_ComboHit`
4. Repeat for `BP_AIBoxer`.

### MetaSound (Optional)

For richer impact audio, replace individual `USoundWave` assets with a MetaSound:

1. Right-click in `Content/LastBell/Sounds/Impacts/` > **Sounds > MetaSound Source**.
2. Name it `MS_PunchImpact`.
3. In the MetaSound editor, create an input `float ImpactForce` (0.0–1.0).
4. Route `ImpactForce` to:
   - A **Random Pitch** node (range 0.9–1.1) on `SW_ImpactPrimary`.
   - A **Volume** multiplier on `SW_ImpactFlesh` (louder at high force).
   - A **Gate** that only plays `SW_ImpactHeavy` when `ImpactForce > 0.7`.
5. Assign `MS_PunchImpact` to the **Impact Sound Primary** slot in the component.

### Level Audio

In `L_FightArena`:
- Add an **Ambient Sound** actor with `SW_CrowdAmbience` set to loop.
- The `BP_CrowdActor` can modulate the crowd volume via a `UAudioComponent` property tied to the crowd react delegate.

In `L_MainMenu` and `L_CharacterSelect`:
- Add an **Ambient Sound** actor with `SW_MenuMusic` looping at low volume.

---

## 16. Final Checklist

Work through this list top-to-bottom before calling the project shippable. Check each item in order — items later in the list depend on earlier ones working correctly.

### Build & Compile

- [ ] C++ compiles with zero errors in both Development Editor and Development configurations.
- [ ] All Blueprint nodes resolve (no red `?` nodes indicating missing classes or functions).
- [ ] `DA_GameSettings`, all four Data Assets, and `IMC_Boxing` are saved and not in an error state.

### Navigation

- [ ] Launch the editor in **Play In Editor (PIE)** mode set to `L_MainMenu`.
- [ ] Main Menu loads — title text visible, Play and Quit buttons respond to clicks.
- [ ] Clicking Play opens `L_CharacterSelect` — three opponent panels display with names and portraits.
- [ ] Clicking an opponent stores the selection in `GameInstance` and opens `L_FightArena`.

### Fight Startup

- [ ] Player spawns at the correct spawn point facing the AI.
- [ ] AI spawns, AI Controller possesses it, and the correct Behavior Tree starts running (verify in **Behavior Tree Debugger**: Window > Developer Tools > Behavior Tree).
- [ ] `WBP_HUD` is visible — health bars, stamina bars, round timer, and round indicator all display correctly.
- [ ] Round timer counts down from 120 seconds.

### Player Controls

- [ ] A / D moves the player left and right within ring bounds.
- [ ] S ducks (duck state persists while held).
- [ ] Space triggers a dodge (brief invincibility + position shift).
- [ ] LMB fires a jab — animation plays through Startup → Active → Recovery phases.
- [ ] RMB fires a hook.
- [ ] E fires an uppercut.
- [ ] Attacks cost stamina — stamina bar decreases.
- [ ] Stamina regenerates when not attacking.

### Hit Detection & Feedback

- [ ] Landing a jab on the AI reduces the AI's health bar.
- [ ] Hit pause triggers on successful hit (brief freeze ~60 ms).
- [ ] Camera shake fires on impact.
- [ ] Impact audio plays (layered if heavyweight).
- [ ] Knockback (`LaunchCharacter`) pushes the AI backward on hit.
- [ ] Combo counter increments when consecutive hits land within the combo window.
- [ ] `WBP_HUD` combo text is visible for combos of 2+.

### AI Behavior

- [ ] Rookie advances and attacks with visible startup delay — easiest to read.
- [ ] Counter opponent dodges when the player attacks, then counter-punches.
- [ ] Heavyweight moves slowly but hits extremely hard — verify 35-damage uppercut one-shots low health.
- [ ] AI respects ring bounds — does not walk through ropes.
- [ ] Fatigue system visible on heavyweight: it rests after several heavy punches.

### Knockdown & KO

- [ ] When AI health reaches 0, knockdown animation plays and `WBP_KnockdownCount` appears.
- [ ] Countdown from 3 → 2 → 1 ticks at 1-second intervals.
- [ ] On third knockdown (or if not beaten on count), KO triggers.
- [ ] KO slow-motion engages: smooth ramp-in (0.15 s), hold at 0.25x scale (1.5 s), ramp-out (0.4 s).
- [ ] `L_WinScreen` opens after KO slow-mo completes.
- [ ] If the player is knocked out three times, `L_LoseScreen` opens.

### Win / Lose Screens

- [ ] `WBP_WinScreen` shows "YOU WIN!" and final stats.
- [ ] Play Again returns to `L_CharacterSelect`.
- [ ] Main Menu returns to `L_MainMenu`.
- [ ] `WBP_LoseScreen` shows "K.O.!" and stats.
- [ ] Rematch re-opens `L_FightArena` with the same fighter selected.

### Audio Final Check

- [ ] Impact sounds play on every successful hit.
- [ ] Breathing loop pitch increases as stamina decreases.
- [ ] Bell sound plays at round start and round end.
- [ ] Crowd ambience loops in the fight arena.
- [ ] Menu music loops on main menu and character select screens.
- [ ] No audio pops, clipping, or missing sound warnings in the Output Log.

### Performance

- [ ] Crowd actor renders 120 instances in a single draw call — confirm in **Stat SceneRendering** (console: `stat scenerendering`): instanced draw call count should not scale with crowd size.
- [ ] Frame rate stays above 60 fps during KO slow-motion (time dilation should not affect render performance).
- [ ] No memory warnings in the Output Log during a full fight loop (start → fight → KO → win screen → character select → fight again).

---

*End of Setup Guide. If any section is blocked by a missing C++ symbol, verify the class exists in `Source/LastBell/` and that the project has been compiled successfully before attempting Blueprint setup.*
