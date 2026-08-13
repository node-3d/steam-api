import { createRequire } from 'node:module';
import { getBin } from '@node-3d/addon-tools';
import '@node-3d/segfault';

export type TSteamId = string & { readonly __steamId: unique symbol };
export type TGameId = string & { readonly __gameId: unique symbol };
export type TSteamLobbyId = TSteamId & { readonly __steamLobbyId: unique symbol };
export type TSteamPublishedFileId = string & {
	readonly __steamPublishedFileId: unique symbol;
};
export type TSteamUgcHandle = string & { readonly __steamUgcHandle: unique symbol };
export type TSteamUgcUpdateHandle = string & {
	readonly __steamUgcUpdateHandle: unique symbol;
};

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

export type TSteamFriendMessage = Readonly<{
	message: string;
	chatEntryType: number;
}>;

export type TSteamFriendGameInfo = Readonly<{
	gameId: TGameId;
	gameIp: number;
	gamePort: number;
	queryPort: number;
	steamIdLobby: TSteamLobbyId;
}>;

export type TSteamLobbyCreated = Readonly<{
	result: number;
	lobbyId: TSteamLobbyId;
}>;

export type TSteamLobbyEnter = Readonly<{
	lobbyId: TSteamLobbyId;
	chatPermissions: number;
	locked: boolean;
	response: number;
}>;

export type TSteamLobbyMatchList = Readonly<{
	lobbiesMatching: number;
}>;

export type TSteamLobbyData = Readonly<{
	key: string;
	value: string;
}>;

export type TSteamLobbyChatEntry = Readonly<{
	steamIdUser: TSteamId;
	data: Buffer;
	chatEntryType: number;
}>;

export type TSteamP2PPacket = Readonly<{
	data: Buffer;
	steamIdRemote: TSteamId;
	messageSize: number;
}>;

export type TSteamP2PSessionConnectionState = Readonly<{
	connectionActive: boolean;
	connecting: boolean;
	sessionError: number;
	usingRelay: boolean;
	bytesQueuedForSend: number;
	packetsQueuedForSend: number;
	remoteIp: number;
	remotePort: number;
}>;

export type TSteamP2PSessionState = Readonly<{
	result: boolean;
	connectionState: TSteamP2PSessionConnectionState;
}>;

export type TSteamUgcItemInstallInfo = Readonly<{
	sizeOnDisk: string;
	folder: string;
	timestamp: number;
}>;

export type TSteamUgcQueryOptions = Readonly<{
	appId?: number;
	page?: number;
	requiredTags?: string[];
	excludedTags?: string[];
	requiredTagGroups?: string[][];
	matchAnyTag?: boolean;
	returnMetadata?: boolean;
	returnLongDescription?: boolean;
	returnAdditionalPreviews?: boolean;
	returnChildren?: boolean;
	returnKeyValueTags?: boolean;
	returnTotalOnly?: boolean;
	language?: string;
	allowCachedResponseMaxAgeSeconds?: number;
}>;

export type TSteamUgcAdditionalPreview = Readonly<{
	urlOrVideoId: string;
	originalFileName: string;
	previewType: number;
}>;

export type TSteamUgcKeyValueTag = Readonly<{
	key: string;
	value: string;
}>;

export type TSteamUgcDetails = Readonly<{
	acceptedForUse: boolean;
	banned: boolean;
	tagsTruncated: boolean;
	fileType: number;
	result: number;
	visibility: number;
	score: number;
	file: TSteamUgcHandle;
	fileName: string;
	fileSize: number;
	previewFile: TSteamUgcHandle;
	previewFileSize: number;
	steamIdOwner: TSteamId;
	consumerAppId: number;
	creatorAppId: number;
	publishedFileId: TSteamPublishedFileId;
	title: string;
	description: string;
	url: string;
	tags: string;
	timeAddedToUserList: number;
	timeCreated: number;
	timeUpdated: number;
	votesDown: number;
	votesUp: number;
	numChildren: number;
	totalFilesSize: string;
	metadata?: string;
	children?: TSteamPublishedFileId[];
	additionalPreviews?: TSteamUgcAdditionalPreview[];
	keyValueTags?: TSteamUgcKeyValueTag[];
}>;

export type TSteamUgcQueryResult = Readonly<{
	result: number;
	totalMatchingResults: number;
	cachedData: boolean;
	nextCursor: string;
	items: TSteamUgcDetails[];
}>;

export type TSteamUgcDownloadResult = Readonly<{
	result: number;
	file: TSteamUgcHandle;
	appId: number;
	sizeInBytes: number;
	fileName: string;
	steamIdOwner: TSteamId;
	path: string;
}>;

export type TSteamUgcSubscribeItemResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
}>;

export type TSteamUgcCreateItemResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	userNeedsToAcceptWorkshopLegalAgreement: boolean;
}>;

export type TSteamUgcSubmitItemUpdateResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	userNeedsToAcceptWorkshopLegalAgreement: boolean;
}>;

export type TSteamUgcItemUpdateProgress = Readonly<{
	status: number;
	bytesProcessed: string;
	bytesTotal: string;
}>;

export type TSteamUgcItemDownloadInfo = Readonly<{
	bytesDownloaded: string;
	bytesTotal: string;
}>;

export type TSteamUgcSetUserItemVoteResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	voteUp: boolean;
}>;

export type TSteamUgcGetUserItemVoteResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	votedUp: boolean;
	votedDown: boolean;
	voteSkipped: boolean;
}>;

export type TSteamUgcFavoriteItemsListChangedResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	wasAddRequest: boolean;
}>;

export type TSteamUgcUnsubscribeResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
}>;

export type TSteamUgcSaveFilesToCloudResult = Readonly<{
	files: string[];
}>;

export type TSteamCloudQuota = Readonly<{
	totalBytes: string;
	availableBytes: string;
}>;

export type TSteamCloudFile = Readonly<{
	name: string;
	size: number;
}>;

export type TSteamCloudSaveFilesResult = Readonly<{
	files: string[];
}>;

export type TSteamIdNamespace = Readonly<{
	isAnonymous: (steamId: TSteamId) => boolean;
	isAnonymousGameServer: (steamId: TSteamId) => boolean;
	isAnonymousGameServerLogin: (steamId: TSteamId) => boolean;
	isAnonymousUser: (steamId: TSteamId) => boolean;
	isChatAccount: (steamId: TSteamId) => boolean;
	isClanAccount: (steamId: TSteamId) => boolean;
	isConsoleUserAccount: (steamId: TSteamId) => boolean;
	isContentServerAccount: (steamId: TSteamId) => boolean;
	isGameServerAccount: (steamId: TSteamId) => boolean;
	isIndividualAccount: (steamId: TSteamId) => boolean;
	isPersistentGameServerAccount: (steamId: TSteamId) => boolean;
	isLobby: (steamId: TSteamId) => boolean;
	getAccountId: (steamId: TSteamId) => number;
	getRawSteamId: (steamId: TSteamId) => TSteamId;
	getAccountType: (steamId: TSteamId) => number;
	isValid: (steamId: TSteamId) => boolean;
	getStaticAccountKey: (steamId: TSteamId) => string;
	getPersonaName: (steamId: TSteamId) => string;
	getNickname: (steamId: TSteamId) => string;
	getRelationship: (steamId: TSteamId) => number;
	getSteamLevel: (steamId: TSteamId) => number;
}>;

export type TSteamWorkshopPublishOptions = Readonly<{
	appId?: number;
	tags?: string[];
	visibility?: number;
	fileType?: number;
}>;

export type TSteamWorkshopUpdateOptions = Readonly<{
	tags?: string[];
	visibility?: number;
	changeDescription?: string;
}>;

export type TSteamUgcFileShareResult = Readonly<{
	result: number;
	file: TSteamUgcHandle;
	fileName: string;
}>;

export type TSteamWorkshopPublishResult = Readonly<{
	result: number;
	publishedFileId: TSteamPublishedFileId;
	userNeedsToAcceptWorkshopLegalAgreement: boolean;
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
	| 'DisabledLocally'
>;

export type TSteamItemUpdateStatus = TSteamEnum<
	| 'Invalid'
	| 'PreparingConfig'
	| 'PreparingContent'
	| 'UploadingContent'
	| 'UploadingPreviewFile'
	| 'CommittingChanges'
>;

export type TSteamItemPreviewType = TSteamEnum<
	| 'Image'
	| 'YouTubeVideo'
	| 'Sketchfab'
	| 'EnvironmentMapHorizontalCross'
	| 'EnvironmentMapLatLong'
	| 'Clip'
	| 'ReservedMax'
>;

export type TSteamRemoteStoragePublishedFileVisibility = TSteamEnum<
	'Public' | 'FriendsOnly' | 'Private' | 'Unlisted'
>;

export type TSteamWorkshopFileType = TSteamEnum<
	| 'Community'
	| 'Microtransaction'
	| 'Collection'
	| 'Art'
	| 'Video'
	| 'Screenshot'
	| 'Game'
	| 'Software'
	| 'Concept'
	| 'WebGuide'
	| 'IntegratedGuide'
	| 'Merch'
	| 'ControllerBinding'
	| 'SteamworksAccessInvite'
	| 'SteamVideo'
	| 'GameManagedItem'
	| 'Clip'
>;

export type TSteamFloatingGamepadTextInputMode = TSteamEnum<
	'SingleLine' | 'MultipleLines' | 'Email' | 'Numeric'
>;

export type TSteamP2PSendType = TSteamEnum<
	'Unreliable' | 'UnreliableNoDelay' | 'Reliable' | 'ReliableWithBuffering'
>;

export type TSteamBeginAuthSessionResult = TSteamEnum<
	| 'OK'
	| 'InvalidTicket'
	| 'DuplicateRequest'
	| 'InvalidVersion'
	| 'GameMismatch'
	| 'ExpiredTicket'
>;

export type TSteamAuthSessionResponse = TSteamEnum<
	| 'OK'
	| 'UserNotConnectedToSteam'
	| 'NoLicenseOrExpired'
	| 'VacBanned'
	| 'LoggedInElsewhere'
	| 'VacCheckTimedOut'
	| 'AuthTicketCanceled'
	| 'AuthTicketInvalidAlreadyUsed'
	| 'AuthTicketInvalid'
	| 'PublisherIssuedBan'
	| 'AuthTicketNetworkIdentityFailure'
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
			type: 'auth-ticket-for-web-api-response';
			handle: number;
			result: number;
			ticketSize: number;
	  }>
	| Readonly<{
			type: 'validate-auth-ticket';
			steamId: TSteamId;
			authSessionResponse: number;
			ownerSteamId: TSteamId;
	  }>
	| Readonly<{
			type: 'game-overlay-activated';
			active: boolean;
			userInitiated: boolean;
			appId: number;
			overlayPid: number;
	  }>
	| Readonly<{
			type: 'persona-state-change';
			steamId: TSteamId;
			changeFlags: number;
	  }>
	| Readonly<{
			type: 'avatar-image-loaded';
			steamId: TSteamId;
			image: number;
			width: number;
			height: number;
	  }>
	| Readonly<{
			type: 'game-connected-friend-chat-message';
			steamId: TSteamId;
			messageId: number;
	  }>
	| Readonly<{
			type: 'rich-presence-join-requested';
			steamIdFriend: TSteamId;
			connect: string;
	  }>
	| Readonly<{
			type: 'lobby-created';
			result: number;
			lobbyId: TSteamLobbyId;
	  }>
	| Readonly<{
			type: 'lobby-data-update';
			lobbyId: TSteamLobbyId;
			steamIdMember: TSteamId;
			success: boolean;
	  }>
	| Readonly<{
			type: 'lobby-enter';
			lobbyId: TSteamLobbyId;
			chatPermissions: number;
			locked: boolean;
			response: number;
	  }>
	| Readonly<{
			type: 'lobby-invite';
			steamIdUser: TSteamId;
			lobbyId: TSteamLobbyId;
			gameId: TGameId;
	  }>
	| Readonly<{
			type: 'lobby-join-requested';
			lobbyId: TSteamLobbyId;
			steamIdFriend: TSteamId;
	  }>
	| Readonly<{
			type: 'lobby-match-list';
			lobbiesMatching: number;
	  }>
	| Readonly<{
			type: 'lobby-chat-update';
			lobbyId: TSteamLobbyId;
			steamIdUserChanged: TSteamId;
			steamIdMakingChange: TSteamId;
			stateChange: number;
	  }>
	| Readonly<{
			type: 'lobby-chat-msg';
			lobbyId: TSteamLobbyId;
			steamIdUser: TSteamId;
			chatEntryType: number;
			chatId: number;
	  }>
	| Readonly<{
			type: 'p2p-session-request';
			steamIdRemote: TSteamId;
	  }>
	| Readonly<{
			type: 'p2p-session-connect-fail';
			steamIdRemote: TSteamId;
			sessionError: number;
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
	  }>
	| Readonly<{
			type: 'download-item-result';
			appId: number;
			publishedFileId: TSteamPublishedFileId;
			result: number;
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
	getAuthSessionTicket: () => TSteamAuthSessionTicket | null;
	getAuthTicketForWebApi: (identity?: string | null) => Promise<TSteamAuthSessionTicket>;
	cancelAuthTicket: (handle: number) => void;
	beginAuthSession: (ticket: Buffer, steamId: TSteamId) => number;
	endAuthSession: (steamId: TSteamId) => void;
	requestEncryptedAppTicket: (userData: Buffer | string) => Promise<Buffer>;
	getEncryptedAppTicket: () => Buffer | null;
	decryptAppTicket: (encryptedTicket: Buffer, key: Buffer) => Buffer | null;
	isTicketForApp: (decryptedTicket: Buffer, appId: number) => boolean;
	getTicketIssueTime: (decryptedTicket: Buffer) => number;
	getTicketSteamId: (decryptedTicket: Buffer) => TSteamId;
	getTicketAppId: (decryptedTicket: Buffer) => number;
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
	showFloatingGamepadTextInput: (
		keyboardMode: number,
		x: number,
		y: number,
		width: number,
		height: number,
	) => boolean;
	dismissFloatingGamepadTextInput: () => boolean;
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
	getFriendCount: (friendFlag: number) => number;
	getFriends: (friendFlag: number) => TSteamId[];
	requestUserInformation: (steamId: TSteamId, requireNameOnly: boolean) => boolean;
	getSmallFriendAvatar: (steamId: TSteamId) => number;
	getMediumFriendAvatar: (steamId: TSteamId) => number;
	getLargeFriendAvatar: (steamId: TSteamId) => number;
	setListenForFriendsMessage: (interceptEnabled: boolean) => boolean;
	replyToFriendMessage: (steamId: TSteamId, message: string) => boolean;
	getFriendMessage: (
		steamId: TSteamId,
		messageId: number,
		maximumMessageSize: number,
	) => TSteamFriendMessage | null;
	getFriendPersonaName: (steamId: TSteamId) => string;
	setRichPresence: (key: string, value: string) => boolean;
	clearRichPresence: () => void;
	getFriendRichPresence: (steamId: TSteamId, key: string) => string;
	setPlayedWith: (steamId: TSteamId) => void;
	getFriendGamePlayed: (steamId: TSteamId) => TSteamFriendGameInfo | null;
	activateGameOverlayInviteDialog: (steamIdLobby: TSteamLobbyId) => void;
	activateGameOverlayToUser: (dialog: string, steamId: TSteamId) => void;
}>;

export type TSteamCloudNamespace = Readonly<{
	saveTextToFile: (fileName: string, content: string) => boolean;
	readTextFromFile: (fileName: string) => string | null;
	deleteFile: (fileName: string) => boolean;
	saveFilesToCloud: (filePaths: string[]) => TSteamCloudSaveFilesResult;
	isEnabledForUser: () => boolean;
	isEnabled: () => boolean;
	setEnabled: (enabled: boolean) => void;
	getQuota: () => TSteamCloudQuota | null;
	getFileCount: () => number;
	getFileNameAndSize: (index: number) => TSteamCloudFile | null;
}>;

export type TSteamMatchmakingNamespace = Readonly<{
	createLobby: (lobbyType: number, maxMembers: number) => Promise<TSteamLobbyCreated>;
	deleteLobbyData: (lobbyId: TSteamLobbyId, key: string) => boolean;
	getLobbyByIndex: (index: number) => TSteamLobbyId | null;
	getLobbyData: (lobbyId: TSteamLobbyId, key: string) => string;
	getLobbyMemberByIndex: (lobbyId: TSteamLobbyId, index: number) => TSteamId | null;
	getNumLobbyMembers: (lobbyId: TSteamLobbyId) => number;
	getLobbyOwner: (lobbyId: TSteamLobbyId) => TSteamId | null;
	inviteUserToLobby: (lobbyId: TSteamLobbyId, inviteeId: TSteamId) => boolean;
	joinLobby: (lobbyId: TSteamLobbyId) => Promise<TSteamLobbyEnter>;
	leaveLobby: (lobbyId: TSteamLobbyId) => void;
	setLobbyData: (lobbyId: TSteamLobbyId, key: string, value: string) => boolean;
	setLobbyJoinable: (lobbyId: TSteamLobbyId, joinable: boolean) => boolean;
	setLobbyOwner: (lobbyId: TSteamLobbyId, ownerId: TSteamId) => boolean;
	setLobbyType: (lobbyId: TSteamLobbyId, lobbyType: number) => boolean;
	requestLobbyList: () => Promise<TSteamLobbyMatchList>;
	getLobbyMemberLimit: (lobbyId: TSteamLobbyId) => number;
	setLobbyMemberLimit: (lobbyId: TSteamLobbyId, limit: number) => boolean;
	getLobbyMemberData: (lobbyId: TSteamLobbyId, memberId: TSteamId, key: string) => string;
	setLobbyMemberData: (lobbyId: TSteamLobbyId, key: string, value: string) => void;
	getLobbyDataCount: (lobbyId: TSteamLobbyId) => number;
	getLobbyDataByIndex: (lobbyId: TSteamLobbyId, index: number) => TSteamLobbyData | null;
	sendLobbyChatMsg: (lobbyId: TSteamLobbyId, data: Buffer) => boolean;
	getLobbyChatEntry: (
		lobbyId: TSteamLobbyId,
		chatId: number,
		maximumMessageSize?: number,
	) => TSteamLobbyChatEntry | null;
}>;

export type TSteamNetworkingNamespace = Readonly<{
	sendP2PPacket: (steamId: TSteamId, sendType: number, data: Buffer, channel?: number) => boolean;
	isP2PPacketAvailable: (channel?: number) => number;
	readP2PPacket: (size: number, channel?: number) => TSteamP2PPacket | null;
	acceptP2PSessionWithUser: (steamId: TSteamId) => boolean;
	getP2PSessionState: (steamId: TSteamId) => TSteamP2PSessionState;
	closeP2PSessionWithUser: (steamId: TSteamId) => boolean;
	closeP2PChannelWithUser: (steamId: TSteamId, channel: number) => boolean;
	isBehindNat: () => boolean;
}>;

export type TSteamNativeUgcNamespace = Readonly<{
	showOverlay: (publishedFileId?: TSteamPublishedFileId) => void;
	getItems: (
		options: TSteamUgcQueryOptions | null | undefined,
		matchingType: number,
		queryType: number,
	) => Promise<TSteamUgcQueryResult>;
	getUserItems: (
		options: TSteamUgcQueryOptions | null | undefined,
		matchingType: number,
		sortOrder: number,
		list: number,
	) => Promise<TSteamUgcQueryResult>;
	createItem: (appId?: number, fileType?: number) => Promise<TSteamUgcCreateItemResult>;
	startItemUpdate: (
		appId: number,
		publishedFileId: TSteamPublishedFileId,
	) => TSteamUgcUpdateHandle;
	setItemTitle: (updateHandle: TSteamUgcUpdateHandle, title: string) => boolean;
	setItemDescription: (updateHandle: TSteamUgcUpdateHandle, description: string) => boolean;
	setItemMetadata: (updateHandle: TSteamUgcUpdateHandle, metadata: string) => boolean;
	setItemVisibility: (updateHandle: TSteamUgcUpdateHandle, visibility: number) => boolean;
	setItemTags: (
		updateHandle: TSteamUgcUpdateHandle,
		tags: string[],
		allowAdminTags?: boolean,
	) => boolean;
	setItemContent: (updateHandle: TSteamUgcUpdateHandle, contentFolder: string) => boolean;
	setItemPreview: (updateHandle: TSteamUgcUpdateHandle, previewFile: string) => boolean;
	submitItemUpdate: (
		updateHandle: TSteamUgcUpdateHandle,
		changeNote?: string,
	) => Promise<TSteamUgcSubmitItemUpdateResult>;
	getItemUpdateProgress: (updateHandle: TSteamUgcUpdateHandle) => TSteamUgcItemUpdateProgress;
	setUserItemVote: (
		publishedFileId: TSteamPublishedFileId,
		voteUp: boolean,
	) => Promise<TSteamUgcSetUserItemVoteResult>;
	getUserItemVote: (
		publishedFileId: TSteamPublishedFileId,
	) => Promise<TSteamUgcGetUserItemVoteResult>;
	addItemToFavorites: (
		publishedFileId: TSteamPublishedFileId,
		appId?: number,
	) => Promise<TSteamUgcFavoriteItemsListChangedResult>;
	removeItemFromFavorites: (
		publishedFileId: TSteamPublishedFileId,
		appId?: number,
	) => Promise<TSteamUgcFavoriteItemsListChangedResult>;
	subscribeItem: (
		publishedFileId: TSteamPublishedFileId,
	) => Promise<TSteamUgcSubscribeItemResult>;
	unsubscribeItem: (
		publishedFileId: TSteamPublishedFileId,
	) => Promise<TSteamUgcUnsubscribeResult>;
	getNumSubscribedItems: (includeLocallyDisabled?: boolean) => number;
	getSubscribedItems: (
		maxEntries?: number,
		includeLocallyDisabled?: boolean,
	) => TSteamPublishedFileId[];
	getItemDownloadInfo: (
		publishedFileId: TSteamPublishedFileId,
	) => TSteamUgcItemDownloadInfo | null;
	downloadItem: (publishedFileId: TSteamPublishedFileId, highPriority?: boolean) => boolean;
	download: (file: TSteamUgcHandle, downloadDir: string) => Promise<TSteamUgcDownloadResult>;
	unsubscribe: (publishedFileId: TSteamPublishedFileId) => Promise<TSteamUgcUnsubscribeResult>;
	saveFilesToCloud: (filePaths: string[]) => TSteamUgcSaveFilesToCloudResult;
	fileShare: (filePath: string) => Promise<TSteamUgcFileShareResult>;
	publishWorkshopFile: (
		options: TSteamWorkshopPublishOptions | null | undefined,
		filePath: string,
		previewFilePath: string,
		title: string,
		description: string,
	) => Promise<TSteamWorkshopPublishResult>;
	updatePublishedWorkshopFile: (
		options: TSteamWorkshopUpdateOptions | null | undefined,
		publishedFileId: TSteamPublishedFileId,
		filePath: string,
		previewFilePath: string,
		title: string,
		description: string,
	) => Promise<TSteamWorkshopPublishResult>;
	getItemState: (publishedFileId: TSteamPublishedFileId) => number;
	getItemInstallInfo: (publishedFileId: TSteamPublishedFileId) => TSteamUgcItemInstallInfo | null;
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
	getNumberOfCurrentPlayers: () => Promise<number>;
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
	| 'k_EUniverseDev'
	| 'EncryptedAppTicketSymmetricKeyLength';

type TNativeNamespaces = Readonly<{
	steam: TSteamNamespace;
	callbacks: TSteamCallbacksNamespace;
	steamId: TSteamIdNamespace;
	user: TSteamUserNamespace;
	utils: TSteamUtilsNamespace;
	apps: TSteamAppsNamespace;
	friends: TSteamFriendsNamespace;
	matchmaking: TSteamMatchmakingNamespace;
	networking: TSteamNetworkingNamespace;
	cloud: TSteamCloudNamespace;
	ugc: TSteamNativeUgcNamespace;
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
	ItemUpdateStatus: TSteamItemUpdateStatus;
	ItemPreviewType: TSteamItemPreviewType;
	RemoteStoragePublishedFileVisibility: TSteamRemoteStoragePublishedFileVisibility;
	WorkshopFileType: TSteamWorkshopFileType;
	FloatingGamepadTextInputMode: TSteamFloatingGamepadTextInputMode;
	P2PSendType: TSteamP2PSendType;
	BeginAuthSessionResult: TSteamBeginAuthSessionResult;
	AuthSessionResponse: TSteamAuthSessionResponse;
}>;

export type TNative = TNativeNamespaces &
	TNativeEnums &
	Readonly<Record<TNativeConstantName, number>>;

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/steam-api.node`) as TNative;
