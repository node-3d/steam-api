import { native } from './native.ts';

export type {
	TGameId,
	TSteamAchievementUnlockTime,
	TSteamAuthSessionTicket,
	TSteamCallbackEvent,
	TSteamDlcData,
	TSteamId,
	TSteamInitResult,
} from './native.ts';

export const {
	restartAppIfNecessary,
	initEx,
	shutdown,
	isSteamRunning,
	runCallbacks,
	releaseCurrentThreadMemory,
	pollCallbacks,
	getHSteamUser,
	isLoggedOn,
	getSteamId,
	getAuthSessionTicket,
	cancelAuthTicket,
	getAppId,
	getConnectedUniverse,
	getIpCountry,
	getServerRealTime,
	getSteamUiLanguage,
	isOverlayEnabled,
	isSteamInBigPictureMode,
	isSubscribed,
	isSubscribedApp,
	isDlcInstalled,
	isAppInstalled,
	getCurrentGameLanguage,
	getAvailableGameLanguages,
	getDlcCount,
	getDlcDataByIndex,
	getAppInstallDir,
	getStatInt,
	getStatFloat,
	setStatInt,
	setStatFloat,
	getAchievement,
	setAchievement,
	clearAchievement,
	storeStats,
	resetAllStats,
	getNumAchievements,
	getAchievementName,
	getAchievementDisplayAttribute,
	getAchievementAndUnlockTime,
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
	const result = initEx();

	if (!result.ok) {
		throw new Error(`SteamAPI_InitEx failed with ${result.result}: ${result.errorMessage}`);
	}
};

export const update = (): ReturnType<typeof pollCallbacks> => {
	runCallbacks();
	return pollCallbacks();
};
