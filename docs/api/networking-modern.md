# Modern P2P networking

Official references: [ISteamNetworkingSockets](https://partner.steamgames.com/doc/api/ISteamNetworkingSockets) and [ISteamNetworkingMessages](https://partner.steamgames.com/doc/api/ISteamNetworkingMessages).

`networking.sockets` is the modern connection-oriented API. Create a P2P listen
socket with `createListenSocketP2P()`, connect with `connectP2P()`, accept
incoming connection handles with `acceptConnection()`, and use the connection
handle to send or receive messages. `networking.messages` is the modern
connectionless API: use `sendMessageToUser()` and poll a local channel with
`receiveMessagesOnChannel()`.

Both interfaces use decimal-string Steam IDs and Node.js `Buffer` payloads.
Received messages are copied into JavaScript and released by the binding before
they are returned. Reads accept at most 256 messages per call; sent messages
cannot exceed Steam's 512 KiB maximum. Send flags and result values are the
numeric Steamworks constants.

```ts
import { networking } from '@node-3d/steam-api';

networking.messages.sendMessageToUser(peer, Buffer.from('hello'));
for (const message of networking.messages.receiveMessagesOnChannel()) {
	console.log(message.steamIdRemote, message.data.toString());
}
```

The socket API reports connection state through Steam callbacks. Keep pumping
`update()` and call `acceptConnection()` promptly for incoming connections.
`update()` reports `networking-connection-status-changed` with `connection`,
`oldState`, `state`, `steamIdRemote`, and `endReason`. The messages API reports
`networking-messages-session-request` (accept it with `acceptSessionWithUser()`)
and `networking-messages-session-failed`.
