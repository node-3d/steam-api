import { createRequire } from 'node:module';
import { getBin } from '@node-3d/addon-tools';
import '@node-3d/segfault';

export type TSteamId = string & { readonly __steamId: unique symbol };
export type TGameId = string & { readonly __gameId: unique symbol };

export type TSteamInitResult = Readonly<{
	result: number;
	ok: boolean;
	errorMessage: string;
}>;

export type TSteamAuthSessionTicket = Readonly<{
	handle: number;
	ticket: Buffer;
}>;

export type TSteamDlcData = Readonly<{
	appId: number;
	available: boolean;
	name: string;
}>;

export type TSteamAchievementUnlockTime = Readonly<{
	achieved: boolean;
	unlockTime: number;
}>;

export type TSteamCallbackEvent =
	| Readonly<{
			type: 'userStatsReceived';
			gameId: TGameId;
			userId: TSteamId;
			result: number;
			currentProgress: 0;
			maxProgress: 0;
	  }>
	| Readonly<{
			type: 'userStatsStored';
			gameId: TGameId;
			result: number;
			currentProgress: 0;
			maxProgress: 0;
	  }>
	| Readonly<{
			type: 'userAchievementStored';
			gameId: TGameId;
			name: string;
			result: 0;
			currentProgress: number;
			maxProgress: number;
	  }>
	| Readonly<{
			type: 'authSessionTicketResponse';
			result: number;
			currentProgress: number;
			maxProgress: 0;
	  }>;

export type TSteamNamespace = Readonly<{
	restartAppIfNecessary: (appId: number) => boolean;
	initEx: () => TSteamInitResult;
	shutdown: () => void;
	isSteamRunning: () => boolean;
	runCallbacks: () => void;
	releaseCurrentThreadMemory: () => void;
}>;

export type TSteamCallbacksNamespace = Readonly<{
	pollCallbacks: () => TSteamCallbackEvent[];
}>;

export type TSteamUserNamespace = Readonly<{
	getHSteamUser: () => number;
	isLoggedOn: () => boolean;
	getSteamId: () => TSteamId;
	getAuthSessionTicket: () => TSteamAuthSessionTicket;
	cancelAuthTicket: (handle: number) => void;
}>;

export type TSteamUtilsNamespace = Readonly<{
	getAppId: () => number;
	getConnectedUniverse: () => number;
	getIpCountry: () => string;
	getServerRealTime: () => number;
	getSteamUiLanguage: () => string;
	isOverlayEnabled: () => boolean;
	isSteamInBigPictureMode: () => boolean;
}>;

export type TSteamAppsNamespace = Readonly<{
	isSubscribed: () => boolean;
	isSubscribedApp: (appId: number) => boolean;
	isDlcInstalled: (appId: number) => boolean;
	isAppInstalled: (appId: number) => boolean;
	getCurrentGameLanguage: () => string;
	getAvailableGameLanguages: () => string;
	getDlcCount: () => number;
	getDlcDataByIndex: (index: number) => TSteamDlcData | null;
	getAppInstallDir: (appId: number) => string | null;
}>;

export type TSteamUserStatsNamespace = Readonly<{
	getStatInt: (name: string) => number | null;
	getStatFloat: (name: string) => number | null;
	setStatInt: (name: string, value: number) => boolean;
	setStatFloat: (name: string, value: number) => boolean;
	getAchievement: (name: string) => boolean | null;
	setAchievement: (name: string) => boolean;
	clearAchievement: (name: string) => boolean;
	storeStats: () => boolean;
	resetAllStats: (achievementsToo: boolean) => boolean;
	getNumAchievements: () => number;
	getAchievementName: (index: number) => string | null;
	getAchievementDisplayAttribute: (name: string, key: string) => string | null;
	getAchievementAndUnlockTime: (name: string) => TSteamAchievementUnlockTime | null;
}>;

type TNativeConstantName =
	| 'k_ESteamAPIInitResult_OK'
	| 'k_ESteamAPIInitResult_FailedGeneric'
	| 'k_ESteamAPIInitResult_NoSteamClient'
	| 'k_ESteamAPIInitResult_VersionMismatch'
	| 'k_EResultOK'
	| 'k_EResultFail'
	| 'k_EUniverseInvalid'
	| 'k_EUniversePublic'
	| 'k_EUniverseBeta'
	| 'k_EUniverseInternal'
	| 'k_EUniverseDev';

type TNativeNamespaces = Readonly<{
	steam: TSteamNamespace;
	callbacks: TSteamCallbacksNamespace;
	user: TSteamUserNamespace;
	utils: TSteamUtilsNamespace;
	apps: TSteamAppsNamespace;
	userStats: TSteamUserStatsNamespace;
}>;

export type TNative = TNativeNamespaces & Readonly<Record<TNativeConstantName, number>>;

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/steam-api.node`) as TNative;
