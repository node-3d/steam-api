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

export type TSteamImageSize = Readonly<{
	width: number;
	height: number;
}>;

export type TSteamAchievementUnlockTime = Readonly<{
	achieved: boolean;
	unlockTime: number;
}>;

type TSteamEnum<TName extends string> = Readonly<Record<TName, number>>;

export type TSteamFriendFlags = TSteamEnum<
	| 'None'
	| 'Blocked'
	| 'FriendshipRequested'
	| 'Immediate'
	| 'ClanMember'
	| 'OnGameServer'
	| 'RequestingFriendship'
	| 'RequestingInfo'
	| 'Ignored'
	| 'IgnoredFriend'
	| 'ChatMember'
	| 'All'
>;

export type TSteamFriendRelationship = TSteamEnum<
	| 'None'
	| 'Blocked'
	| 'RequestRecipient'
	| 'Friend'
	| 'RequestInitiator'
	| 'Ignored'
	| 'IgnoredFriend'
	| 'Suggested'
>;

export type TSteamPersonaChange = TSteamEnum<
	| 'Name'
	| 'Status'
	| 'ComeOnline'
	| 'GoneOffline'
	| 'GamePlayed'
	| 'GameServer'
	| 'Avatar'
	| 'JoinedSource'
	| 'LeftSource'
	| 'RelationshipChanged'
	| 'NameFirstSet'
	| 'NickName'
	| 'SteamLevel'
>;

export type TSteamAccountType = TSteamEnum<
	| 'Invalid'
	| 'Individual'
	| 'Multiseat'
	| 'GameServer'
	| 'AnonymousGameServer'
	| 'Pending'
	| 'ContentServer'
	| 'Clan'
	| 'Chat'
	| 'ConsoleUser'
	| 'AnonymousUser'
>;

export type TSteamChatEntryType = TSteamEnum<
	| 'Invalid'
	| 'ChatMsg'
	| 'Typing'
	| 'InviteGame'
	| 'Emote'
	| 'LeftConversation'
	| 'Entered'
	| 'WasKicked'
	| 'WasBanned'
	| 'Disconnected'
	| 'HistoricalChat'
	| 'LinkBlocked'
>;

export type TSteamChatMemberStateChange = TSteamEnum<
	'Entered' | 'Left' | 'Disconnected' | 'Kicked' | 'Banned'
>;

export type TSteamLobbyComparison = TSteamEnum<
	'EqualToOrLessThan' | 'LessThan' | 'Equal' | 'GreaterThan' | 'NotEqual' | 'EqualToOrGreaterThan'
>;

export type TSteamLobbyDistanceFilter = TSteamEnum<'Close' | 'Default' | 'Far' | 'Worldwide'>;

export type TSteamLobbyType = TSteamEnum<'Private' | 'FriendsOnly' | 'Public' | 'Invisible'>;

export type TSteamResult = TSteamEnum<
	| 'OK'
	| 'Fail'
	| 'NoConnection'
	| 'InvalidPassword'
	| 'LoggedInElsewhere'
	| 'InvalidProtocolVer'
	| 'InvalidParam'
	| 'FileNotFound'
	| 'Busy'
	| 'InvalidState'
	| 'InvalidName'
	| 'InvalidEmail'
	| 'DuplicateName'
	| 'AccessDenied'
	| 'Timeout'
	| 'Banned'
	| 'AccountNotFound'
	| 'InvalidSteamID'
	| 'ServiceUnavailable'
	| 'NotLoggedOn'
	| 'Pending'
	| 'EncryptionFailure'
	| 'InsufficientPrivilege'
	| 'LimitExceeded'
	| 'Revoked'
	| 'Expired'
	| 'AlreadyRedeemed'
	| 'DuplicateRequest'
	| 'AlreadyOwned'
	| 'IPNotFound'
	| 'PersistFailed'
	| 'LockingFailed'
	| 'LogonSessionReplaced'
	| 'ConnectFailed'
	| 'HandshakeFailed'
	| 'IOFailure'
	| 'RemoteDisconnect'
	| 'ShoppingCartNotFound'
	| 'Blocked'
	| 'Ignored'
	| 'NoMatch'
	| 'AccountDisabled'
	| 'ServiceReadOnly'
	| 'AccountNotFeatured'
	| 'AdministratorOK'
	| 'ContentVersion'
	| 'TryAnotherCM'
	| 'PasswordRequiredToKickSession'
	| 'AlreadyLoggedInElsewhere'
	| 'Suspended'
	| 'Cancelled'
	| 'DataCorruption'
	| 'DiskFull'
	| 'RemoteCallFailed'
	| 'PasswordUnset'
	| 'ExternalAccountUnlinked'
	| 'PSNTicketInvalid'
	| 'ExternalAccountAlreadyLinked'
	| 'RemoteFileConflict'
	| 'IllegalPassword'
	| 'SameAsPreviousValue'
	| 'AccountLogonDenied'
	| 'CannotUseOldPassword'
	| 'InvalidLoginAuthCode'
	| 'AccountLogonDeniedNoMail'
	| 'HardwareNotCapableOfIPT'
	| 'IPTInitError'
	| 'ParentalControlRestricted'
	| 'FacebookQueryError'
	| 'ExpiredLoginAuthCode'
	| 'IPLoginRestrictionFailed'
	| 'AccountLockedDown'
	| 'AccountLogonDeniedVerifiedEmailRequired'
	| 'NoMatchingURL'
	| 'BadResponse'
	| 'RequirePasswordReEntry'
	| 'ValueOutOfRange'
	| 'UnexpectedError'
	| 'Disabled'
	| 'InvalidCEGSubmission'
	| 'RestrictedDevice'
	| 'RegionLocked'
	| 'RateLimitExceeded'
	| 'AccountLoginDeniedNeedTwoFactor'
	| 'ItemDeleted'
	| 'AccountLoginDeniedThrottle'
	| 'TwoFactorCodeMismatch'
	| 'TwoFactorActivationCodeMismatch'
	| 'AccountAssociatedToMultiplePartners'
	| 'NotModified'
	| 'NoMobileDevice'
	| 'TimeNotSynced'
	| 'SmsCodeFailed'
	| 'AccountLimitExceeded'
	| 'AccountActivityLimitExceeded'
	| 'PhoneActivityLimitExceeded'
	| 'RefundToWallet'
	| 'EmailSendFailure'
	| 'NotSettled'
	| 'NeedCaptcha'
	| 'GSLTDenied'
	| 'GSOwnerDenied'
	| 'InvalidItemType'
	| 'IPBanned'
	| 'GSLTExpired'
	| 'InsufficientFunds'
	| 'TooManyPending'
	| 'NoSiteLicensesFound'
	| 'WGNetworkSendExceeded'
	| 'AccountNotFriends'
	| 'LimitedUserAccount'
	| 'CantRemoveItem'
	| 'AccountDeleted'
	| 'ExistingUserCancelledLicense'
	| 'CommunityCooldown'
	| 'NoLauncherSpecified'
	| 'MustAgreeToSSA'
	| 'LauncherMigrated'
	| 'SteamRealmMismatch'
	| 'InvalidSignature'
	| 'ParseFailure'
	| 'NoVerifiedPhone'
	| 'InsufficientBattery'
	| 'ChargerRequired'
	| 'CachedCredentialInvalid'
>;

export type TSteamUgcMatchingType = TSteamEnum<
	| 'Items'
	| 'ItemsMtx'
	| 'ItemsReadyToUse'
	| 'Collections'
	| 'Artwork'
	| 'Videos'
	| 'Screenshots'
	| 'AllGuides'
	| 'WebGuides'
	| 'IntegratedGuides'
	| 'UsableInGame'
	| 'ControllerBindings'
>;

export type TSteamUgcQueryType = TSteamEnum<
	| 'RankedByVote'
	| 'RankedByPublicationDate'
	| 'AcceptedForGameRankedByAcceptanceDate'
	| 'RankedByTrend'
	| 'FavoritedByFriendsRankedByPublicationDate'
	| 'CreatedByFriendsRankedByPublicationDate'
	| 'RankedByNumTimesReported'
	| 'CreatedByFollowedUsersRankedByPublicationDate'
	| 'NotYetRated'
	| 'RankedByTotalVotesAsc'
	| 'RankedByVotesUp'
	| 'RankedByTextSearch'
>;

export type TSteamUserUgcList = TSteamEnum<
	| 'Published'
	| 'VotedOn'
	| 'VotedUp'
	| 'VotedDown'
	| 'WillVoteLater'
	| 'Favorited'
	| 'Subscribed'
	| 'UsedOrPlayer'
	| 'Followed'
>;

export type TSteamUserUgcListSortOrder = TSteamEnum<
	| 'CreationOrderDesc'
	| 'CreationOrderAsc'
	| 'TitleAsc'
	| 'LastUpdatedDesc'
	| 'SubscriptionDateDesc'
	| 'VoteScoreDesc'
	| 'ForModeration'
>;

export type TSteamUgcItemState = TSteamEnum<
	| 'None'
	| 'Subscribed'
	| 'LegacyItem'
	| 'Installed'
	| 'NeedsUpdate'
	| 'Downloading'
	| 'DownloadPending'
>;

export type TSteamFloatingGamepadTextInputMode = TSteamEnum<
	'SingleLine' | 'MultipleLines' | 'Email' | 'Numeric'
>;

export type TSteamP2PSendType = TSteamEnum<
	'Unreliable' | 'UnreliableNoDelay' | 'Reliable' | 'ReliableWithBuffering'
>;

export type TSteamCallbackEvent =
	| Readonly<{
			type: 'user-stats-received';
			gameId: TGameId;
			userId: TSteamId;
			result: number;
	  }>
	| Readonly<{
			type: 'user-stats-stored';
			gameId: TGameId;
			result: number;
	  }>
	| Readonly<{
			type: 'user-achievement-stored';
			gameId: TGameId;
			name: string;
			currentProgress: number;
			maxProgress: number;
	  }>
	| Readonly<{
			type: 'auth-session-ticket-response';
			handle: number;
			result: number;
	  }>
	| Readonly<{
			type: 'game-overlay-activated';
			active: boolean;
			userInitiated: boolean;
			appId: number;
			overlayPid: number;
	  }>
	| Readonly<{
			type: 'steam-servers-connected';
	  }>
	| Readonly<{
			type: 'steam-servers-disconnected';
			result: number;
	  }>
	| Readonly<{
			type: 'steam-server-connect-failure';
			result: number;
			stillRetrying: boolean;
	  }>
	| Readonly<{
			type: 'steam-shutdown';
	  }>
	| Readonly<{
			type: 'dlc-installed';
			appId: number;
	  }>
	| Readonly<{
			type: 'new-url-launch-parameters';
	  }>
	| Readonly<{
			type: 'floating-gamepad-text-input-dismissed';
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
	getImageSize: (image: number) => TSteamImageSize | null;
	getImageRgba: (image: number) => Buffer | null;
	isOverlayEnabled: () => boolean;
	isSteamInBigPictureMode: () => boolean;
	isSteamRunningOnSteamDeck: () => boolean;
}>;

export type TSteamAppsNamespace = Readonly<{
	isSubscribed: () => boolean;
	isSubscribedApp: (appId: number) => boolean;
	isDlcInstalled: (appId: number) => boolean;
	isAppInstalled: (appId: number) => boolean;
	installDlc: (appId: number) => void;
	uninstallDlc: (appId: number) => void;
	getAppBuildId: () => number;
	getCurrentGameLanguage: () => string;
	getAvailableGameLanguages: () => string;
	getCurrentGameInstallDir: () => string | null;
	getDlcCount: () => number;
	getDlcDataByIndex: (index: number) => TSteamDlcData | null;
	getAppInstallDir: (appId: number) => string | null;
	getLaunchCommandLine: () => string;
}>;

export type TSteamFriendsNamespace = Readonly<{
	activateGameOverlay: (dialog: string) => void;
	activateGameOverlayToWebPage: (url: string, mode?: number) => void;
	activateGameOverlayToStore: (appId: number, flag: number) => void;
}>;

export type TSteamUserStatsNamespace = Readonly<{
	getStatInt: (name: string) => number | null;
	getStatFloat: (name: string) => number | null;
	setStat: (name: string, value: number) => boolean;
	setStatInt: (name: string, value: number) => boolean;
	setStatFloat: (name: string, value: number) => boolean;
	getAchievement: (name: string) => boolean | null;
	setAchievement: (name: string) => boolean;
	clearAchievement: (name: string) => boolean;
	indicateAchievementProgress: (
		name: string,
		currentProgress: number,
		maxProgress: number,
	) => boolean;
	storeStats: () => boolean;
	resetAllStats: (achievementsToo: boolean) => boolean;
	getNumAchievements: () => number;
	getAchievementNames: () => (string | null)[];
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
	friends: TSteamFriendsNamespace;
	userStats: TSteamUserStatsNamespace;
}>;

type TNativeEnums = Readonly<{
	FriendFlags: TSteamFriendFlags;
	FriendRelationship: TSteamFriendRelationship;
	PersonaChange: TSteamPersonaChange;
	AccountType: TSteamAccountType;
	ChatEntryType: TSteamChatEntryType;
	ChatMemberStateChange: TSteamChatMemberStateChange;
	LobbyComparison: TSteamLobbyComparison;
	LobbyDistanceFilter: TSteamLobbyDistanceFilter;
	LobbyType: TSteamLobbyType;
	Result: TSteamResult;
	UGCMatchingType: TSteamUgcMatchingType;
	UGCQueryType: TSteamUgcQueryType;
	UserUGCList: TSteamUserUgcList;
	UserUGCListSortOrder: TSteamUserUgcListSortOrder;
	UGCItemState: TSteamUgcItemState;
	FloatingGamepadTextInputMode: TSteamFloatingGamepadTextInputMode;
	P2PSendType: TSteamP2PSendType;
}>;

export type TNative = TNativeNamespaces &
	TNativeEnums &
	Readonly<Record<TNativeConstantName, number>>;

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/steam-api.node`) as TNative;
