# Steam API Examples

These examples are Node-first consumer programs for `@node-3d/steam-api`.
They intentionally do not use Electron or NW.js hosts.

Run examples from the package checkout with Node's TypeScript support:

```powershell
node examples/basic/read-only/basic.ts
node examples/basic/read-only/events.ts --seconds=30
node examples/basic/read-write/greenworks-sample.ts
```

Most examples require the Steam client to be running and an app context that
matches the account and files being exercised. Operations that change user
state or publish Workshop content require explicit flags such as
`--write-achievement`, `--write-stats`, or `--yes-create-workshop-item`.

## Layout

- `basic/read-only` - terminal-only examples that inspect Steam state without
  changing account, app, network, cloud, or Workshop state.
- `basic/read-write` - terminal-only examples that create, update, delete,
  publish, subscribe, send, or otherwise mutate Steam or local example state.
- `visual` - reserved for examples that render with Node3D packages such as
  `@node-3d/core`. Electron and NW.js examples do not belong here.
- `spacewar` - helper for running examples against Valve's Steamworks test app,
  AppID `480`, from a folder-local `steam_appid.txt`.
