import assert from 'node:assert/strict';
import test from 'node:test';
import { callbacks, steam, steamId } from '@node-3d/steam-api';

test('loads the packed Steamworks addon and runtime libraries', () => {
	assert.equal(typeof steam.initEx, 'function');
	assert.equal(typeof callbacks.pollCallbacks, 'function');
	assert.equal(typeof steamId.isValid, 'function');
});
