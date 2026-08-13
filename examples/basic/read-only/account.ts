import { steamId, user } from '@node-3d/steam-api';
import { initSteam } from '../helpers.ts';

initSteam();

const id = user.getSteamId();

console.log('Steam ID:', id);
console.log('Logged on:', user.isLoggedOn());
console.log('HSteamUser:', user.getHSteamUser());
console.log('Account ID:', steamId.getAccountId(id));
console.log('Raw Steam ID:', steamId.getRawSteamId(id));
console.log('Account type:', steamId.getAccountType(id));
console.log('Valid:', steamId.isValid(id));
console.log('Individual:', steamId.isIndividualAccount(id));
console.log('Anonymous:', steamId.isAnonymous(id));
console.log('Persona name:', steamId.getPersonaName(id));
console.log('Nickname:', steamId.getNickname(id));
console.log('Relationship:', steamId.getRelationship(id));
console.log('Steam level:', steamId.getSteamLevel(id));
