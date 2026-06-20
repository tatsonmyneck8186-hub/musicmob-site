# LAST BELL — TODO / Known Improvements

Tracked work for taking the code-only prototype toward an Early Access-quality
indie boxing game. Items are ordered roughly by value. Nothing here requires
editor-authored assets unless explicitly marked **[editor]**.

## Reliability / correctness
- [ ] Centralize global time-dilation ownership (hit-pause vs KO slow-mo vs
      knockdown slow-mo) behind a single arbiter to fully prevent any cross-system
      dilation conflict. (Mitigated: finishing blows skip hit-pause.)
- [ ] Disable component ticks (not just actor tick) on the opponent while parked
      in the menu for a bit more idle headroom.
- [ ] Consider a small object pool for `AImpactSparkActor` if spark spawn rate
      ever climbs (currently bounded and cheap for 1v1).

## Game feel / balance
- [ ] Per-archetype tuning pass once playtested (Heavyweight may need a slightly
      shorter recovery; Counter dodge probability vs. readability).
- [ ] Optional "get up before the count" window for knockdowns (currently any
      knockdown is a guaranteed round loss). Keep simple if added.
- [ ] Add subtle camera push-in during the KO slow-motion for more drama.

## Presentation (procedural, no assets)
- [ ] Round-start "ROUND n — FIGHT!" canvas banner during the pre-round intro.
- [ ] Stamina-low vignette / breathing emphasis on the HUD.
- [ ] Crowd color variation (a few team-colored instances) for life.

## Audio
- [ ] Wire the `BoxerAudioComponent` sound slots — currently nullable hooks.
      **[editor]** for real sound assets, but the hooks are in place.

## Editor-only (when you want to go beyond procedural placeholders)
- [ ] **[editor]** Real skeletal meshes (Manny/Quinn) + Animation Blueprint;
      `LoadFighterData` already hides the placeholder rig when a mesh is set.
- [ ] **[editor]** UMG menu/HUD widgets (the C++ HUD exposes
      BlueprintImplementableEvents and a `HUDWidgetClass` slot to swap in).
- [ ] **[editor]** Niagara punch/impact VFX to replace the procedural spark.

## Future-proofing
- [ ] Keep gameplay state authoritative on the game mode for eventual networking;
      avoid storing match-critical state on the HUD. (Currently respected.)
