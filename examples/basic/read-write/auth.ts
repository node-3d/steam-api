import { Buffer } from 'node:buffer';
import { EncryptedAppTicketSymmetricKeyLength, user } from '@node-3d/steam-api';
import { hasFlag, initSteam, option, waitForSteamCall } from '../helpers.ts';

initSteam();

const sessionTicket = user.getAuthSessionTicket();
console.log('Auth session ticket:', sessionTicket);
if (sessionTicket) {
	user.cancelAuthTicket(sessionTicket.handle);
	console.log('Cancelled session ticket:', sessionTicket.handle);
}

if (hasFlag('web-api')) {
	const identity = option('identity') ?? null;
	const ticket = await waitForSteamCall(user.getAuthTicketForWebApi(identity));
	console.log('Web API auth ticket:', {
		handle: ticket.handle,
		ticketBytes: ticket.ticket.byteLength,
	});
	user.cancelAuthTicket(ticket.handle);
}

if (hasFlag('encrypted')) {
	const userData = option('user-data') ?? 'node3d';
	const encryptedTicket = await waitForSteamCall(user.requestEncryptedAppTicket(userData));
	console.log('Encrypted app ticket bytes:', encryptedTicket.byteLength);

	const keyHex = option('key-hex');
	if (keyHex) {
		const key = Buffer.from(keyHex, 'hex');
		if (key.byteLength !== EncryptedAppTicketSymmetricKeyLength) {
			throw new Error(
				`Encrypted app ticket key must be ${EncryptedAppTicketSymmetricKeyLength} bytes.`,
			);
		}

		const decrypted = user.decryptAppTicket(encryptedTicket, key);
		console.log('Decrypted app ticket:', decrypted);
		if (decrypted) {
			console.log('Ticket app id:', user.getTicketAppId(decrypted));
			console.log('Ticket Steam ID:', user.getTicketSteamId(decrypted));
			console.log('Ticket issue time:', user.getTicketIssueTime(decrypted));
		}
	}
}
