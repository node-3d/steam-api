import { Buffer } from 'node:buffer';
import { P2PSendType, networking } from '@node-3d/steam-api';
import { initSteam, numberOption, option, pumpFor, steamIdFromOption } from '../helpers.ts';

initSteam();

const peer = option('peer') ? steamIdFromOption('peer') : null;
const channel = numberOption('channel', 0);

if (peer) {
	const message = option('message') ?? `hello from node3d at ${new Date().toISOString()}`;
	console.log(
		'Send P2P packet:',
		networking.sendP2PPacket(peer, P2PSendType.Reliable, Buffer.from(message), channel),
	);
	console.log('Session state:', networking.getP2PSessionState(peer));
}

await pumpFor(numberOption('seconds', 30) * 1000, (event) => {
	console.log('event', event);
	if (event.type === 'p2p-session-request') {
		console.log(
			'Accept P2P session:',
			event.steamIdRemote,
			networking.acceptP2PSessionWithUser(event.steamIdRemote),
		);
	}

	let packetSize = networking.isP2PPacketAvailable(channel);
	while (packetSize > 0) {
		const packet = networking.readP2PPacket(packetSize, channel);
		console.log('P2P packet:', packet);
		packetSize = networking.isP2PPacketAvailable(channel);
	}
});

if (peer) {
	console.log('Close P2P channel:', networking.closeP2PChannelWithUser(peer, channel));
	console.log('Close P2P session:', networking.closeP2PSessionWithUser(peer));
}

console.log('Behind NAT:', networking.isBehindNat());
