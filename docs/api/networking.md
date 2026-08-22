# Legacy P2P networking

Official reference: [ISteamNetworking](https://partner.steamgames.com/doc/api/ISteamNetworking).

`networking` wraps Steam's legacy P2P packet interface. Send a Node.js `Buffer`
with `sendP2PPacket(steamId, P2PSendType.Reliable, data, channel?)`; the boolean
return value says whether Steam accepted the packet for transmission.

On a `p2p-session-request` event, decide whether to accept the peer before
calling `acceptP2PSessionWithUser(steamId)`. Use `getP2PSessionState()` for the
current state, `closeP2PSessionWithUser()` when the peer disconnects, and
`closeP2PChannelWithUser()` to close one channel.

```ts
const size = networking.isP2PPacketAvailable(channel);
if (size > 0) {
	const packet = networking.readP2PPacket(size, channel);
	if (packet) {
		handlePacket(packet.steamIdRemote, packet.data);
	}
}
```

`readP2PPacket()` returns `null` when no packet is available. Supply the size
reported by `isP2PPacketAvailable()`; the binding caps requested reads before
allocating native memory. `isBehindNat()` is a best-effort Steam connectivity
signal, not a complete network diagnosis.

