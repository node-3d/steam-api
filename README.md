# @node-3d/steam-api

[WIP] Steamworks API bindings for Node.js.

## Install

```bash
npm install @node-3d/steam-api
```

Published packages include prebuilt native binaries.
A consumer install doesn't need a compiler or a local Steamworks SDK copy.

## Example

```ts
import { getAppId, getSteamId, initEx, isLoggedOn, update } from '@node-3d/steam-api';

const result = initEx();

if (!result.ok) {
	throw new Error(`Steamworks init failed: ${result.errorMessage}`);
}

console.log('app', getAppId());
console.log('user', getSteamId(), isLoggedOn() ? 'logged on' : 'offline');

for (const event of update()) {
	console.log(event);
}
```

## API Surface

The initial binding covers lifecycle, callback pumping, basic app/user helpers,
auth tickets, DLC metadata, and user stats/achievements.

Core lifecycle:

- `restartAppIfNecessary(appId)`
- `initEx()`
- `init()`
- `shutdown()`
- `isSteamRunning()`
- `runCallbacks()`
- `pollCallbacks()`
- `update()`
- `releaseCurrentThreadMemory()`

User/app/utils helpers:

- `getSteamId()`
- `isLoggedOn()`
- `getAuthSessionTicket()`
- `cancelAuthTicket(handle)`
- `getAppId()`
- `getConnectedUniverse()`
- `getIpCountry()`
- `getServerRealTime()`
- `getSteamUiLanguage()`
- `isOverlayEnabled()`
- `isSubscribed()`
- `isDlcInstalled(appId)`
- `getDlcDataByIndex(index)`

Stats and achievements:

- `getStatInt(name)`
- `getStatFloat(name)`
- `setStatInt(name, value)`
- `setStatFloat(name, value)`
- `getAchievement(name)`
- `setAchievement(name)`
- `clearAchievement(name)`
- `storeStats()`
- `resetAllStats(achievementsToo)`
- `getAchievementAndUnlockTime(name)`

`update()` runs Steam callbacks and returns queued typed callback payloads such
as `userStatsReceived`, `userStatsStored`, and `userAchievementStored`.

## Steamworks SDK Notice

Valve's Steamworks SDK files are not part of this repository.
This repository contains Node3D binding code only.

Maintainers must provide the SDK separately under Valve's Steamworks
terms when building native binaries.

See [BUILD.md](BUILD.md) for local and CI build setup.
