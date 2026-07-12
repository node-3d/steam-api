import { native } from './native.ts';

export type {
	TGameId,
	TSteamAchievementUnlockTime,
	TSteamAppsNamespace,
	TSteamAuthSessionTicket,
	TSteamCallbacksNamespace,
	TSteamCallbackEvent,
	TSteamDlcData,
	TSteamId,
	TSteamInitResult,
	TSteamNamespace,
	TSteamUserNamespace,
	TSteamUserStatsNamespace,
	TSteamUtilsNamespace,
} from './native.ts';

export const {
	steam,
	callbacks,
	user,
	utils,
	apps,
	userStats,
	k_ESteamAPIInitResult_OK,
	k_ESteamAPIInitResult_FailedGeneric,
	k_ESteamAPIInitResult_NoSteamClient,
	k_ESteamAPIInitResult_VersionMismatch,
	k_EResultOK,
	k_EResultFail,
	k_EUniverseInvalid,
	k_EUniversePublic,
	k_EUniverseBeta,
	k_EUniverseInternal,
	k_EUniverseDev,
} = native;

export const init = (): void => {
	const result = steam.initEx();

	if (!result.ok) {
		throw new Error(`SteamAPI_InitEx failed with ${result.result}: ${result.errorMessage}`);
	}
};

export const update = (): ReturnType<typeof callbacks.pollCallbacks> => {
	steam.runCallbacks();
	return callbacks.pollCallbacks();
};
