# Contributing to Steam API

`@node-3d/steam-api` targets capability parity with the Greenworks implementation
vendored under `.tmp/greenworks`, not drop-in API compatibility.

A Greenworks item is covered when an application can achieve the same outcome
with `@node-3d/steam-api`, even if the Node3D API uses different names, grouped
namespaces, direct return values, typed result objects, promises, or explicit
callback polling.

## Greenworks Audit Sources

- `.tmp/greenworks/greenworks.js`
- `.tmp/greenworks/src/api/*.cc`
- `.tmp/greenworks/src/steam_id.*`
- `.tmp/greenworks/docs/*.md`
- Current `@node-3d/steam-api` public surface in `ts/index.ts`, `ts/native.ts`,
  and `src/cpp/*.cpp`

## API Shape

- Keep the primary Node3D API as typed ESM exports.
- Keep public APIs re-exported from `ts/index.ts`.
- Prefer grouped namespaces such as `steam`, `callbacks`, `user`, `utils`,
  `apps`, and `userStats` over a flat Greenworks-style module.
- Keep Greenworks migration mappings in `GREENWORKS.md`.
- Do not add a drop-in Greenworks facade unless the package direction changes
  explicitly.
- For each Greenworks callback-style API, decide whether the Node3D outcome is
  best exposed through direct return values, promises, or
  `callbacks.pollCallbacks()`.

## Implementation Milestones

- [ ] Phase 1: close easy parity gaps in existing namespaces.
  - `getAppBuildId`
  - `isSteamRunningOnSteamDeck`
  - UI language alias/mapping decision
  - `getCurrentGameInstallDir`
  - overlay activation helpers
  - image size/RGBA helpers
  - DLC install/uninstall
  - `indicateAchievementProgress`
  - `getAchievementNames`
  - `setStat` convenience helper or documented split helpers
- [ ] Phase 2: add typed enum exports and result/object types.
- [ ] Phase 3: expand callback bridge to cover Greenworks event families.
- [ ] Phase 4: implement friends, SteamID object parity or string-first
  equivalents, avatars, chat, and rich presence.
- [ ] Phase 5: implement remote storage/cloud and callback-style async wrappers.
- [ ] Phase 6: implement matchmaking/lobbies and P2P networking.
- [ ] Phase 7: implement workshop/UGC and archive utility helpers if still
  desired.
- [ ] Phase 8: keep `GREENWORKS.md` migration mappings current for implemented
  capability families.

## Verification Per Capability

- Native bindings validate argument count and types before touching Steamworks
  inputs.
- 64-bit Steam identifiers are returned as strings or opaque typed objects,
  never unsafe JavaScript numbers.
- TypeScript declarations match C++ return shapes exactly.
- Tests cover argument validation and no-Steam/no-initialization behavior where
  possible.
- Steam-dependent tests are skipped or guarded when Steam/SDK/runtime is not
  available.
- README and `GREENWORKS.md` examples are updated after each public API family
  lands.
