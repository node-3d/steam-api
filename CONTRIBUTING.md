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

- [x] Phase 1: close easy parity gaps in existing namespaces.
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
- [x] Phase 2: add typed enum exports.
- [ ] Phase 2b: add typed result/object types for larger API families.
  - [x] UGC published file ID, UGC handle, and item install info types.
  - [x] UGC query details and async query result types.
  - [x] UGC download and unsubscribe result types.
  - [x] UGC file share, publish, and update result types.
  - [x] UGC composed publish/update result types.
  - [x] UGC sync result types.
- [x] Phase 3a: refactor callback bridge payloads and cover core Greenworks
  event outcomes.
- [ ] Phase 3b: expand callback bridge as friends, lobbies, P2P, UGC, and auth
  event families land.
  - [x] Friends callback events.
  - [x] Lobby callback events.
  - [x] P2P networking callback events.
  - [x] Auth ticket callback events.
- [ ] Phase 4: implement friends, SteamID object parity or string-first
  equivalents, avatars, chat, and rich presence.
  - [x] SteamID string-first helper namespace.
  - [x] Friends, avatars, chat message, rich presence, and played-with methods.
- [ ] Phase 5: implement remote storage/cloud and callback-style async wrappers.
  - [x] Cloud text/file writes, reads, delete, settings, quota, and enumeration.
- [ ] Phase 6: implement matchmaking/lobbies and P2P networking.
  - [x] Matchmaking lobby methods.
  - [x] P2P packet/session methods.
- [x] Phase 6b: implement auth session and encrypted app ticket helpers.
- [x] Phase 6c: implement current player count and floating gamepad text input.
- [x] Phase 7: implement workshop/UGC equivalents.
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
