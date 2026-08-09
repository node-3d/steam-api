# @node-3d/steam-api

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fsteam-api.svg)](https://badge.fury.io/js/@node-3d%2Fsteam-api)
[![Lint](https://github.com/node-3d/steam-api/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/steam-api/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/test.yml)

[WIP] Steamworks API bindings for Node.js.

## Install

```bash
npm install @node-3d/steam-api
```



Published packages include prebuilt native binaries.
A consumer install doesn't need a compiler or a local Steamworks SDK copy.

## Example

```ts
import { steam, update, user, utils } from '@node-3d/steam-api';

const result = steam.initEx();

if (!result.ok) {
	throw new Error(`Steamworks init failed: ${result.errorMessage}`);
}

console.log('app', utils.getAppId());
console.log('user', user.getSteamId(), user.isLoggedOn() ? 'logged on' : 'offline');

for (const event of update()) {
	console.log(event);
}
```

## API Surface

Migrating from Greenworks? See [GREENWORKS.md](GREENWORKS.md) for capability
mappings and migration notes.

The initial binding covers lifecycle, callback pumping, basic app/user helpers,
auth tickets, DLC metadata, and user stats/achievements.

Methods are grouped by Steamworks interface rather than exported as one flat
module. This keeps names close to Valve's API documentation and avoids a second
Node3D-specific taxonomy for the full Steamworks surface.

`steam`:

- `steam.restartAppIfNecessary(appId)`
- `steam.initEx()`
- `steam.shutdown()`
- `steam.isSteamRunning()`
- `steam.runCallbacks()`
- `steam.releaseCurrentThreadMemory()`

Top-level helpers:

- `init()` - calls `steam.initEx()` and throws when initialization fails.
- `update()` - calls `steam.runCallbacks()` and then `callbacks.pollCallbacks()`.

`callbacks`:

- `callbacks.pollCallbacks()`

`user`:

- `user.getHSteamUser()`
- `user.getSteamId()`
- `user.isLoggedOn()`
- `user.getAuthSessionTicket()`
- `user.cancelAuthTicket(handle)`

`utils`:

- `utils.getAppId()`
- `utils.getConnectedUniverse()`
- `utils.getIpCountry()`
- `utils.getServerRealTime()`
- `utils.getSteamUiLanguage()`
- `utils.isOverlayEnabled()`
- `utils.isSteamInBigPictureMode()`

`apps`:

- `apps.isSubscribed()`
- `apps.isSubscribedApp(appId)`
- `apps.isDlcInstalled(appId)`
- `apps.isAppInstalled(appId)`
- `apps.getCurrentGameLanguage()`
- `apps.getAvailableGameLanguages()`
- `apps.getDlcCount()`
- `apps.getDlcDataByIndex(index)`
- `apps.getAppInstallDir(appId)`

`userStats`:

- `userStats.getStatInt(name)`
- `userStats.getStatFloat(name)`
- `userStats.setStatInt(name, value)`
- `userStats.setStatFloat(name, value)`
- `userStats.getAchievement(name)`
- `userStats.setAchievement(name)`
- `userStats.clearAchievement(name)`
- `userStats.storeStats()`
- `userStats.resetAllStats(achievementsToo)`
- `userStats.getNumAchievements()`
- `userStats.getAchievementName(index)`
- `userStats.getAchievementDisplayAttribute(name, key)`
- `userStats.getAchievementAndUnlockTime(name)`

`steam.runCallbacks()` maps to `SteamAPI_RunCallbacks`.
`callbacks.pollCallbacks()` drains the Node3D callback queue.
`update()` does both and returns queued typed callback payloads:

- `userStatsReceived`: `{ gameId, userId, result }`
- `userStatsStored`: `{ gameId, result }`
- `userAchievementStored`: `{ gameId, name, currentProgress, maxProgress }`
- `authSessionTicketResponse`: `{ result, currentProgress }`

Steam IDs and game IDs are exposed as branded strings because Steamworks uses
64-bit identifiers that should not be rounded through JavaScript numbers.

## Steamworks SDK Notice

Valve's Steamworks SDK files are not part of this repository.
This repository contains Node3D binding code only.

Maintainers must provide the SDK separately under Valve's Steamworks
terms when building native binaries.

See [BUILD.md](BUILD.md) for local and CI build setup.

## Binary Origin

Release archives are built by this repository's public GitHub Actions workflows.

Attestations: https://github.com/node-3d/steam-api/attestations

To verify a downloaded archive:

```bash
gh release download <tag> -R node-3d/steam-api -p <platform>.gz
gh attestation verify <platform>.gz -R node-3d/steam-api
```
