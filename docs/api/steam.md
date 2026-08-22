# Steam lifecycle namespace

Official background: [Steamworks API overview](https://partner.steamgames.com/doc/sdk/api).

`steam` is the direct lifecycle namespace:

- `restartAppIfNecessary(appId)` returns `true` when Steam initiated a restart;
  exit the current process in that case.
- `initEx()` returns `{ ok, result, errorMessage }` rather than throwing. The
  top-level `init()` helper calls it and throws for failed initialization.
- `isSteamRunning()` is safe before initialization and answers whether the
  Steam client is running.
- `runCallbacks()` drives Steam callback delivery. Prefer top-level `update()`
  in ordinary applications because it also returns typed callback events.
- `releaseCurrentThreadMemory()` performs Steam's optional thread-local cleanup.
- `shutdown()` ends the client API session. Do not call Steam namespaces after
  shutdown unless the process initializes Steam again.

See [Lifecycle and callbacks](lifecycle.md) for promise completion and event
delivery patterns.

