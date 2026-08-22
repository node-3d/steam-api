# Lifecycle and callbacks

Official background: [Steamworks API overview](https://partner.steamgames.com/doc/sdk/api).

Call `steam.restartAppIfNecessary(appId)` before initialization when Steam should
relaunch a production build. Then call `init()` (throwing convenience helper) or
`steam.initEx()` (structured `{ ok, result, errorMessage }` result) exactly once
for the process. Call `steam.shutdown()` during orderly process shutdown.

```ts
import { init, steam, update } from '@node-3d/steam-api';

init();

setInterval(() => {
	for (const event of update()) {
		console.log(event.type);
	}
}, 16);
```

`update()` runs `steam.runCallbacks()` and drains `callbacks.pollCallbacks()`.
Use `steam.runCallbacks()` plus `callbacks.pollCallbacks()` separately only when
the application needs its own scheduling. `releaseCurrentThreadMemory()` is an
optional Steam cleanup call for long-lived worker threads.

## Promises need callback pumping

Methods such as `matchmaking.createLobby()`, `ugc.getItems()`,
`user.getAuthTicketForWebApi()`, and `userStats.getNumberOfCurrentPlayers()`
resolve from Steam call results. Awaiting them without pumping callbacks leaves
them pending. A simple helper for scripts is:

```ts
import { update } from '@node-3d/steam-api';

export async function waitForSteamCall<T>(promise: Promise<T>): Promise<T> {
	let done = false;
	promise.finally(() => {
		done = true;
	}).catch(() => {});

	while (!done) {
		update();
		await new Promise((resolve) => setTimeout(resolve, 16));
	}

	return promise;
}
```

`downloadItem()` is intentionally different: it returns `boolean` synchronously
to indicate whether Steam accepted the download request. Observe its completion
through the `download-item-result` event returned by `update()`.

