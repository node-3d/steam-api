import { native } from './native.ts';

export type {
	TGameId,
	TSteamAccountType,
	TSteamAchievementUnlockTime,
	TSteamAppsNamespace,
	TSteamAuthSessionTicket,
	TSteamCallbacksNamespace,
	TSteamCallbackEvent,
	TSteamChatEntryType,
	TSteamChatMemberStateChange,
	TSteamDlcData,
	TSteamFloatingGamepadTextInputMode,
	TSteamFriendsNamespace,
	TSteamFriendFlags,
	TSteamFriendRelationship,
	TSteamId,
	TSteamImageSize,
	TSteamInitResult,
	TSteamLobbyComparison,
	TSteamLobbyDistanceFilter,
	TSteamLobbyType,
	TSteamNamespace,
	TSteamP2PSendType,
	TSteamPersonaChange,
	TSteamResult,
	TSteamUgcItemState,
	TSteamUgcMatchingType,
	TSteamUgcQueryType,
	TSteamUserNamespace,
	TSteamUserStatsNamespace,
	TSteamUserUgcList,
	TSteamUserUgcListSortOrder,
	TSteamUtilsNamespace,
} from './native.ts';

export const {
	steam,
	callbacks,
	user,
	utils,
	apps,
	friends,
	userStats,
	FriendFlags,
	FriendRelationship,
	PersonaChange,
	AccountType,
	ChatEntryType,
	ChatMemberStateChange,
	LobbyComparison,
	LobbyDistanceFilter,
	LobbyType,
	Result,
	UGCMatchingType,
	UGCQueryType,
	UserUGCList,
	UserUGCListSortOrder,
	UGCItemState,
	FloatingGamepadTextInputMode,
	P2PSendType,
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
