# Contributing to Steam API

`@node-3d/steam-api` initially targeted capability parity with the Greenworks
implementation vendored under `.tmp/greenworks`, not drop-in API compatibility.
That milestone is complete. Current Steamworks coverage work is tracked in
`docs/CHECKLIST.md`.

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
- Public method names should follow the actual Steamworks method being bound.
- Keep Greenworks migration mappings in `docs/GREENWORKS.md`.
- Keep Steamworks coverage and current milestone status in `docs/CHECKLIST.md`.
- Do not add a drop-in Greenworks facade unless the package direction changes
  explicitly.
- For each Greenworks callback-style API, decide whether the Node3D outcome is
  best exposed through direct return values, promises, or
  `callbacks.pollCallbacks()`.
- Legacy compatibility wrappers should be documented in `docs/GREENWORKS.md`.
- `ISteamRemoteStorage::UGCDownload` should not own the public
  `ugc.downloadItem()` name long term if `ISteamUGC::DownloadItem` is added.
- Prefer `ugc.download()` or `ugc.downloadLegacy()` for the legacy
  `UGCDownload`/`UGCRead` helper if a rename is needed.

## Implementation Milestones

- [x] Greenworks capability parity.
- [x] Native addon CI parity with core Node3D native addons.
- [x] No-Steam hosted CI runtime coverage for safe native calls.
- [ ] Modern Steamworks UGC and Workshop coverage. See `docs/CHECKLIST.md`.

## Verification Per Capability

- Native bindings validate argument count and types before touching Steamworks
  inputs.
- TypeScript declarations match C++ return shapes exactly.
- 64-bit Steam identifiers are returned as branded decimal strings, never unsafe
  JavaScript numbers.
- Tests cover argument validation and no-Steam/no-initialization behavior where
  possible.
- Steam-dependent tests are skipped or guarded when Steam/SDK/runtime is not
  available.
- README, `docs/CHECKLIST.md`, and `docs/GREENWORKS.md` are updated after each
  public API family lands.
