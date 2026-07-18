# @node-3d/steam-api

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fsteam-api.svg)](https://badge.fury.io/js/@node-3d/steam-api)
[![Lint](https://github.com/node-3d/steam-api/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/steam-api/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/steam-api/actions/workflows/test.yml)

[WIP] Steamworks API bindings for Node.js.

## Install

```bash
npm install @node-3d/steam-api
```

## Binary Provenance

Release binary archives for this package are published by public GitHub Actions
workflows.
When a release asset includes a GitHub Artifact Attestation, you can verify that
the downloaded archive was published by the visible workflow in this repository:

```console
gh release download <tag> -R node-3d/steam-api -p <platform>.gz
gh attestation verify <platform>.gz -R node-3d/steam-api
```

The attestation links the archive to the repository, workflow, commit, and
release build identity. It does not replace review of the source code, workflow,
or third-party library contents.

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

The initial binding covers lifecycle, callback pumping, basic app/user helpers,
auth tickets, DLC metadata, and user stats/achievements.

`steam`:

- `steam.restartAppIfNecessary(appId)`
- `steam.initEx()`
- `init()`
- `steam.shutdown()`
- `steam.isSteamRunning()`
- `steam.runCallbacks()`
- `update()`
- `steam.releaseCurrentThreadMemory()`

`callbacks`:

- `callbacks.pollCallbacks()`

`user`:

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

`apps`:

- `apps.isSubscribed()`
- `apps.isDlcInstalled(appId)`
- `apps.getDlcDataByIndex(index)`

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
- `userStats.getAchievementAndUnlockTime(name)`

`update()` runs Steam callbacks and returns queued typed callback payloads such
as `userStatsReceived`, `userStatsStored`, and `userAchievementStored`.

## Steamworks SDK Notice

Valve's Steamworks SDK files are not part of this repository.
This repository contains Node3D binding code only.

Maintainers must provide the SDK separately under Valve's Steamworks
terms when building native binaries.

See [BUILD.md](BUILD.md) for local and CI build setup.
