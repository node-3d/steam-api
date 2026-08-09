import test from 'node:test';
import assert from 'node:assert/strict';

test(
	'native addon contract is loadable when a prebuilt binary is present',
	{ skip: true },
	async () => {
		const steamApi = await import('./index.ts');
		assert.equal(typeof steamApi.steam.initEx, 'function');
		assert.equal(typeof steamApi.callbacks.pollCallbacks, 'function');
		assert.equal(typeof steamApi.friends.activateGameOverlay, 'function');
		assert.equal(Object.isFrozen(steamApi.FriendFlags), true);
		assert.equal(typeof steamApi.LobbyType.Public, 'number');
		assert.equal(typeof steamApi.user.cancelAuthTicket, 'function');
	},
);
