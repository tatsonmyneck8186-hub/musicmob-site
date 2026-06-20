# LAST BELL — Changelog

All notable changes to the LAST BELL prototype. This is a code-only procedural
build (no editor-authored .uasset, UMG, or Niagara required).

## [Unreal 5.7] — Hardening pass: match-flow reliability + KO feel

### Fixed
- **Match-flow timer reliability.** Split the single overloaded `RoundTimerHandle`
  into three dedicated handles: `RoundTimerHandle` (the repeating 1s round clock
  only), `SequenceTimerHandle` (one-shot transitions: intro / between-round /
  finalize), and `KnockdownTimerHandle` (the 10-count). Removes the footgun of one
  handle driving five different delayed jobs.
- **Knockdown vs round-clock race.** `TriggerKnockdown` now stops the round clock
  for the duration of the count, so a knockdown near time-expiry can no longer
  race `EndRound` (round timer) against the knockdown count.
- **Double round-end guard.** `EndRound` now no-ops unless the match is in
  `RoundActive` or `Knockdown`, preventing a stray/buffered timer from ending a
  round twice.
- **KO presentation pop.** A finishing blow now skips the attacker's hit-pause so
  the target's KO/knockdown slow-motion owns the global time scale — eliminates a
  one-frame dilation pop on the knockout.

### Performance
- The opponent is now tick-disabled while parked (hidden) in the menu and
  re-enabled when a fight starts, so it no longer animates/runs movement logic
  invisibly.

## Earlier history (summarized)

- **Priorities 5 & 6:** Procedural arena polish (raised platform, apron, 4 posts,
  3-tier ropes, corner pads, mat logo, tiered bouncing crowd, impact spark actor,
  colored lighting, height fog) and a code-only front end (main menu → opponent
  select → fight → win/lose → rematch/menu).
- **Priorities 1–4:** Composed humanoid fighter bodies, procedural state-driven
  animation, arcade camera (framing/sway/shake/zoom), upgraded canvas HUD.
- **Code-only boot:** Runtime fighter data factory, procedural arena spawning,
  self-contained C++ AI fallback, runtime Enhanced Input, canvas debug HUD.
- **KO/knockdown wiring:** Health-zero KO ends the match (win/lose); KO-meter
  knockdown runs the 10-count and ends the round.
- **Core build:** Full UE 5.7 C++ module — combat (3-phase attacks, hit-pause),
  stats (health/stamina/KO meter), 3 AI archetypes, round/match system, data
  assets, interface-decoupled fighters.
