# Callback events

The `callbacks` namespace has one method: `pollCallbacks()`. It drains and
returns the typed Node3D event queue without calling Steam. The top-level
`update()` helper is usually preferable because it calls `steam.runCallbacks()`
first, then returns the same event array.

```ts
for (const event of update()) {
	switch (event.type) {
		case 'lobby-invite':
			showInvite(event.steamIdUser, event.lobbyId);
			break;
		case 'download-item-result':
			handleWorkshopDownload(event.publishedFileId, event.result);
			break;
	}
}
```

Events are discriminated by `type`, so TypeScript narrows their payloads in a
`switch`. The complete event union is `TSteamCallbackEvent`; it covers stats,
auth, overlay, persona/avatar, lobby, P2P, connection, DLC, gamepad text input,
and Workshop download events. Poll frequently enough for promises and Steam UI
state to progress, but keep application-level work out of the callback pump.

