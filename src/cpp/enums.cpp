#include "enums.hpp"

#include <initializer_list>
#include <utility>

namespace steam_api::enums {
namespace {
using EnumEntries = std::initializer_list<std::pair<const char *, int32_t>>;

Napi::Object makeEnum(Napi::Env env, EnumEntries entries) {
	Napi::Object value = Napi::Object::New(env);
	for (const auto &[name, enumValue] : entries) {
		value.Set(name, enumValue);
	}
	value.Freeze();
	return value;
}

void setEnum(Napi::Env env, Napi::Object exports, const char *name, EnumEntries entries) {
	exports.Set(name, makeEnum(env, entries));
}
} // namespace

void setEnumExports(Napi::Env env, Napi::Object exports) {
	setEnum(
	    env,
	    exports,
	    "FriendFlags",
	    {
	        { "None", k_EFriendFlagNone },
	        { "Blocked", k_EFriendFlagBlocked },
	        { "FriendshipRequested", k_EFriendFlagFriendshipRequested },
	        { "Immediate", k_EFriendFlagImmediate },
	        { "ClanMember", k_EFriendFlagClanMember },
	        { "OnGameServer", k_EFriendFlagOnGameServer },
	        { "RequestingFriendship", k_EFriendFlagRequestingFriendship },
	        { "RequestingInfo", k_EFriendFlagRequestingInfo },
	        { "Ignored", k_EFriendFlagIgnored },
	        { "IgnoredFriend", k_EFriendFlagIgnoredFriend },
	        { "ChatMember", k_EFriendFlagChatMember },
	        { "All", k_EFriendFlagAll },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "FriendRelationship",
	    {
	        { "None", k_EFriendRelationshipNone },
	        { "Blocked", k_EFriendRelationshipBlocked },
	        { "RequestRecipient", k_EFriendRelationshipRequestRecipient },
	        { "Friend", k_EFriendRelationshipFriend },
	        { "RequestInitiator", k_EFriendRelationshipRequestInitiator },
	        { "Ignored", k_EFriendRelationshipIgnored },
	        { "IgnoredFriend", k_EFriendRelationshipIgnoredFriend },
	        { "Suggested", k_EFriendRelationshipSuggested_DEPRECATED },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "PersonaChange",
	    {
	        { "Name", k_EPersonaChangeName },
	        { "Status", k_EPersonaChangeStatus },
	        { "ComeOnline", k_EPersonaChangeComeOnline },
	        { "GoneOffline", k_EPersonaChangeGoneOffline },
	        { "GamePlayed", k_EPersonaChangeGamePlayed },
	        { "GameServer", k_EPersonaChangeGameServer },
	        { "Avatar", k_EPersonaChangeAvatar },
	        { "JoinedSource", k_EPersonaChangeJoinedSource },
	        { "LeftSource", k_EPersonaChangeLeftSource },
	        { "RelationshipChanged", k_EPersonaChangeRelationshipChanged },
	        { "NameFirstSet", k_EPersonaChangeNameFirstSet },
	        { "NickName", k_EPersonaChangeNickname },
	        { "SteamLevel", k_EPersonaChangeSteamLevel },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "AccountType",
	    {
	        { "Invalid", k_EAccountTypeInvalid },
	        { "Individual", k_EAccountTypeIndividual },
	        { "Multiseat", k_EAccountTypeMultiseat },
	        { "GameServer", k_EAccountTypeGameServer },
	        { "AnonymousGameServer", k_EAccountTypeAnonGameServer },
	        { "Pending", k_EAccountTypePending },
	        { "ContentServer", k_EAccountTypeContentServer },
	        { "Clan", k_EAccountTypeClan },
	        { "Chat", k_EAccountTypeChat },
	        { "ConsoleUser", k_EAccountTypeConsoleUser },
	        { "AnonymousUser", k_EAccountTypeAnonUser },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "ChatEntryType",
	    {
	        { "Invalid", k_EChatEntryTypeInvalid },
	        { "ChatMsg", k_EChatEntryTypeChatMsg },
	        { "Typing", k_EChatEntryTypeTyping },
	        { "InviteGame", k_EChatEntryTypeInviteGame },
	        { "Emote", k_EChatEntryTypeEmote },
	        { "LeftConversation", k_EChatEntryTypeLeftConversation },
	        { "Entered", k_EChatEntryTypeEntered },
	        { "WasKicked", k_EChatEntryTypeWasKicked },
	        { "WasBanned", k_EChatEntryTypeWasBanned },
	        { "Disconnected", k_EChatEntryTypeDisconnected },
	        { "HistoricalChat", k_EChatEntryTypeHistoricalChat },
	        { "LinkBlocked", k_EChatEntryTypeLinkBlocked },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "ChatMemberStateChange",
	    {
	        { "Entered", k_EChatMemberStateChangeEntered },
	        { "Left", k_EChatMemberStateChangeLeft },
	        { "Disconnected", k_EChatMemberStateChangeDisconnected },
	        { "Kicked", k_EChatMemberStateChangeKicked },
	        { "Banned", k_EChatMemberStateChangeBanned },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "LobbyComparison",
	    {
	        { "EqualToOrLessThan", k_ELobbyComparisonEqualToOrLessThan },
	        { "LessThan", k_ELobbyComparisonLessThan },
	        { "Equal", k_ELobbyComparisonEqual },
	        { "GreaterThan", k_ELobbyComparisonGreaterThan },
	        { "NotEqual", k_ELobbyComparisonNotEqual },
	        { "EqualToOrGreaterThan", k_ELobbyComparisonEqualToOrGreaterThan },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "LobbyDistanceFilter",
	    {
	        { "Close", k_ELobbyDistanceFilterClose },
	        { "Default", k_ELobbyDistanceFilterDefault },
	        { "Far", k_ELobbyDistanceFilterFar },
	        { "Worldwide", k_ELobbyDistanceFilterWorldwide },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "LobbyType",
	    {
	        { "Private", k_ELobbyTypePrivate },
	        { "FriendsOnly", k_ELobbyTypeFriendsOnly },
	        { "Public", k_ELobbyTypePublic },
	        { "Invisible", k_ELobbyTypeInvisible },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "Result",
	    {
	        { "OK", k_EResultOK },
	        { "Fail", k_EResultFail },
	        { "NoConnection", k_EResultNoConnection },
	        { "InvalidPassword", k_EResultInvalidPassword },
	        { "LoggedInElsewhere", k_EResultLoggedInElsewhere },
	        { "InvalidProtocolVer", k_EResultInvalidProtocolVer },
	        { "InvalidParam", k_EResultInvalidParam },
	        { "FileNotFound", k_EResultFileNotFound },
	        { "Busy", k_EResultBusy },
	        { "InvalidState", k_EResultInvalidState },
	        { "InvalidName", k_EResultInvalidName },
	        { "InvalidEmail", k_EResultInvalidEmail },
	        { "DuplicateName", k_EResultDuplicateName },
	        { "AccessDenied", k_EResultAccessDenied },
	        { "Timeout", k_EResultTimeout },
	        { "Banned", k_EResultBanned },
	        { "AccountNotFound", k_EResultAccountNotFound },
	        { "InvalidSteamID", k_EResultInvalidSteamID },
	        { "ServiceUnavailable", k_EResultServiceUnavailable },
	        { "NotLoggedOn", k_EResultNotLoggedOn },
	        { "Pending", k_EResultPending },
	        { "EncryptionFailure", k_EResultEncryptionFailure },
	        { "InsufficientPrivilege", k_EResultInsufficientPrivilege },
	        { "LimitExceeded", k_EResultLimitExceeded },
	        { "Revoked", k_EResultRevoked },
	        { "Expired", k_EResultExpired },
	        { "AlreadyRedeemed", k_EResultAlreadyRedeemed },
	        { "DuplicateRequest", k_EResultDuplicateRequest },
	        { "AlreadyOwned", k_EResultAlreadyOwned },
	        { "IPNotFound", k_EResultIPNotFound },
	        { "PersistFailed", k_EResultPersistFailed },
	        { "LockingFailed", k_EResultLockingFailed },
	        { "LogonSessionReplaced", k_EResultLogonSessionReplaced },
	        { "ConnectFailed", k_EResultConnectFailed },
	        { "HandshakeFailed", k_EResultHandshakeFailed },
	        { "IOFailure", k_EResultIOFailure },
	        { "RemoteDisconnect", k_EResultRemoteDisconnect },
	        { "ShoppingCartNotFound", k_EResultShoppingCartNotFound },
	        { "Blocked", k_EResultBlocked },
	        { "Ignored", k_EResultIgnored },
	        { "NoMatch", k_EResultNoMatch },
	        { "AccountDisabled", k_EResultAccountDisabled },
	        { "ServiceReadOnly", k_EResultServiceReadOnly },
	        { "AccountNotFeatured", k_EResultAccountNotFeatured },
	        { "AdministratorOK", k_EResultAdministratorOK },
	        { "ContentVersion", k_EResultContentVersion },
	        { "TryAnotherCM", k_EResultTryAnotherCM },
	        { "PasswordRequiredToKickSession", k_EResultPasswordRequiredToKickSession },
	        { "AlreadyLoggedInElsewhere", k_EResultAlreadyLoggedInElsewhere },
	        { "Suspended", k_EResultSuspended },
	        { "Cancelled", k_EResultCancelled },
	        { "DataCorruption", k_EResultDataCorruption },
	        { "DiskFull", k_EResultDiskFull },
	        { "RemoteCallFailed", k_EResultRemoteCallFailed },
	        { "PasswordUnset", k_EResultPasswordUnset },
	        { "ExternalAccountUnlinked", k_EResultExternalAccountUnlinked },
	        { "PSNTicketInvalid", k_EResultPSNTicketInvalid },
	        { "ExternalAccountAlreadyLinked", k_EResultExternalAccountAlreadyLinked },
	        { "RemoteFileConflict", k_EResultRemoteFileConflict },
	        { "IllegalPassword", k_EResultIllegalPassword },
	        { "SameAsPreviousValue", k_EResultSameAsPreviousValue },
	        { "AccountLogonDenied", k_EResultAccountLogonDenied },
	        { "CannotUseOldPassword", k_EResultCannotUseOldPassword },
	        { "InvalidLoginAuthCode", k_EResultInvalidLoginAuthCode },
	        { "AccountLogonDeniedNoMail", k_EResultAccountLogonDeniedNoMail },
	        { "HardwareNotCapableOfIPT", k_EResultHardwareNotCapableOfIPT },
	        { "IPTInitError", k_EResultIPTInitError },
	        { "ParentalControlRestricted", k_EResultParentalControlRestricted },
	        { "FacebookQueryError", k_EResultFacebookQueryError },
	        { "ExpiredLoginAuthCode", k_EResultExpiredLoginAuthCode },
	        { "IPLoginRestrictionFailed", k_EResultIPLoginRestrictionFailed },
	        { "AccountLockedDown", k_EResultAccountLockedDown },
	        { "AccountLogonDeniedVerifiedEmailRequired", k_EResultAccountLogonDeniedVerifiedEmailRequired },
	        { "NoMatchingURL", k_EResultNoMatchingURL },
	        { "BadResponse", k_EResultBadResponse },
	        { "RequirePasswordReEntry", k_EResultRequirePasswordReEntry },
	        { "ValueOutOfRange", k_EResultValueOutOfRange },
	        { "UnexpectedError", k_EResultUnexpectedError },
	        { "Disabled", k_EResultDisabled },
	        { "InvalidCEGSubmission", k_EResultInvalidCEGSubmission },
	        { "RestrictedDevice", k_EResultRestrictedDevice },
	        { "RegionLocked", k_EResultRegionLocked },
	        { "RateLimitExceeded", k_EResultRateLimitExceeded },
	        { "AccountLoginDeniedNeedTwoFactor", k_EResultAccountLoginDeniedNeedTwoFactor },
	        { "ItemDeleted", k_EResultItemDeleted },
	        { "AccountLoginDeniedThrottle", k_EResultAccountLoginDeniedThrottle },
	        { "TwoFactorCodeMismatch", k_EResultTwoFactorCodeMismatch },
	        { "TwoFactorActivationCodeMismatch", k_EResultTwoFactorActivationCodeMismatch },
	        { "AccountAssociatedToMultiplePartners", k_EResultAccountAssociatedToMultiplePartners },
	        { "NotModified", k_EResultNotModified },
	        { "NoMobileDevice", k_EResultNoMobileDevice },
	        { "TimeNotSynced", k_EResultTimeNotSynced },
	        { "SmsCodeFailed", k_EResultSmsCodeFailed },
	        { "AccountLimitExceeded", k_EResultAccountLimitExceeded },
	        { "AccountActivityLimitExceeded", k_EResultAccountActivityLimitExceeded },
	        { "PhoneActivityLimitExceeded", k_EResultPhoneActivityLimitExceeded },
	        { "RefundToWallet", k_EResultRefundToWallet },
	        { "EmailSendFailure", k_EResultEmailSendFailure },
	        { "NotSettled", k_EResultNotSettled },
	        { "NeedCaptcha", k_EResultNeedCaptcha },
	        { "GSLTDenied", k_EResultGSLTDenied },
	        { "GSOwnerDenied", k_EResultGSOwnerDenied },
	        { "InvalidItemType", k_EResultInvalidItemType },
	        { "IPBanned", k_EResultIPBanned },
	        { "GSLTExpired", k_EResultGSLTExpired },
	        { "InsufficientFunds", k_EResultInsufficientFunds },
	        { "TooManyPending", k_EResultTooManyPending },
	        { "NoSiteLicensesFound", k_EResultNoSiteLicensesFound },
	        { "WGNetworkSendExceeded", k_EResultWGNetworkSendExceeded },
	        { "AccountNotFriends", k_EResultAccountNotFriends },
	        { "LimitedUserAccount", k_EResultLimitedUserAccount },
	        { "CantRemoveItem", k_EResultCantRemoveItem },
	        { "AccountDeleted", k_EResultAccountDeleted },
	        { "ExistingUserCancelledLicense", k_EResultExistingUserCancelledLicense },
	        { "CommunityCooldown", k_EResultCommunityCooldown },
	        { "NoLauncherSpecified", k_EResultNoLauncherSpecified },
	        { "MustAgreeToSSA", k_EResultMustAgreeToSSA },
	        { "LauncherMigrated", k_EResultLauncherMigrated },
	        { "SteamRealmMismatch", k_EResultSteamRealmMismatch },
	        { "InvalidSignature", k_EResultInvalidSignature },
	        { "ParseFailure", k_EResultParseFailure },
	        { "NoVerifiedPhone", k_EResultNoVerifiedPhone },
	        { "InsufficientBattery", k_EResultInsufficientBattery },
	        { "ChargerRequired", k_EResultChargerRequired },
	        { "CachedCredentialInvalid", k_EResultCachedCredentialInvalid },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "UGCMatchingType",
	    {
	        { "Items", k_EUGCMatchingUGCType_Items },
	        { "ItemsMtx", k_EUGCMatchingUGCType_Items_Mtx },
	        { "ItemsReadyToUse", k_EUGCMatchingUGCType_Items_ReadyToUse },
	        { "Collections", k_EUGCMatchingUGCType_Collections },
	        { "Artwork", k_EUGCMatchingUGCType_Artwork },
	        { "Videos", k_EUGCMatchingUGCType_Videos },
	        { "Screenshots", k_EUGCMatchingUGCType_Screenshots },
	        { "AllGuides", k_EUGCMatchingUGCType_AllGuides },
	        { "WebGuides", k_EUGCMatchingUGCType_WebGuides },
	        { "IntegratedGuides", k_EUGCMatchingUGCType_IntegratedGuides },
	        { "UsableInGame", k_EUGCMatchingUGCType_UsableInGame },
	        { "ControllerBindings", k_EUGCMatchingUGCType_ControllerBindings },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "UGCQueryType",
	    {
	        { "RankedByVote", k_EUGCQuery_RankedByVote },
	        { "RankedByPublicationDate", k_EUGCQuery_RankedByPublicationDate },
	        { "AcceptedForGameRankedByAcceptanceDate", k_EUGCQuery_AcceptedForGameRankedByAcceptanceDate },
	        { "RankedByTrend", k_EUGCQuery_RankedByTrend },
	        { "FavoritedByFriendsRankedByPublicationDate",
	          k_EUGCQuery_FavoritedByFriendsRankedByPublicationDate },
	        { "CreatedByFriendsRankedByPublicationDate",
	          k_EUGCQuery_CreatedByFriendsRankedByPublicationDate },
	        { "RankedByNumTimesReported", k_EUGCQuery_RankedByNumTimesReported },
	        { "CreatedByFollowedUsersRankedByPublicationDate",
	          k_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate },
	        { "NotYetRated", k_EUGCQuery_NotYetRated },
	        { "RankedByTotalVotesAsc", k_EUGCQuery_RankedByTotalVotesAsc },
	        { "RankedByVotesUp", k_EUGCQuery_RankedByVotesUp },
	        { "RankedByTextSearch", k_EUGCQuery_RankedByTextSearch },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "UserUGCList",
	    {
	        { "Published", k_EUserUGCList_Published },
	        { "VotedOn", k_EUserUGCList_VotedOn },
	        { "VotedUp", k_EUserUGCList_VotedUp },
	        { "VotedDown", k_EUserUGCList_VotedDown },
	        { "WillVoteLater", k_EUserUGCList_WillVoteLater },
	        { "Favorited", k_EUserUGCList_Favorited },
	        { "Subscribed", k_EUserUGCList_Subscribed },
	        { "UsedOrPlayer", k_EUserUGCList_UsedOrPlayed },
	        { "Followed", k_EUserUGCList_Followed },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "UserUGCListSortOrder",
	    {
	        { "CreationOrderDesc", k_EUserUGCListSortOrder_CreationOrderDesc },
	        { "CreationOrderAsc", k_EUserUGCListSortOrder_CreationOrderAsc },
	        { "TitleAsc", k_EUserUGCListSortOrder_TitleAsc },
	        { "LastUpdatedDesc", k_EUserUGCListSortOrder_LastUpdatedDesc },
	        { "SubscriptionDateDesc", k_EUserUGCListSortOrder_SubscriptionDateDesc },
	        { "VoteScoreDesc", k_EUserUGCListSortOrder_VoteScoreDesc },
	        { "ForModeration", k_EUserUGCListSortOrder_ForModeration },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "UGCItemState",
	    {
	        { "None", k_EItemStateNone },
	        { "Subscribed", k_EItemStateSubscribed },
	        { "LegacyItem", k_EItemStateLegacyItem },
	        { "Installed", k_EItemStateInstalled },
	        { "NeedsUpdate", k_EItemStateNeedsUpdate },
	        { "Downloading", k_EItemStateDownloading },
	        { "DownloadPending", k_EItemStateDownloadPending },
	        { "DisabledLocally", k_EItemStateDisabledLocally },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "ItemUpdateStatus",
	    {
	        { "Invalid", k_EItemUpdateStatusInvalid },
	        { "PreparingConfig", k_EItemUpdateStatusPreparingConfig },
	        { "PreparingContent", k_EItemUpdateStatusPreparingContent },
	        { "UploadingContent", k_EItemUpdateStatusUploadingContent },
	        { "UploadingPreviewFile", k_EItemUpdateStatusUploadingPreviewFile },
	        { "CommittingChanges", k_EItemUpdateStatusCommittingChanges },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "ItemPreviewType",
	    {
	        { "Image", k_EItemPreviewType_Image },
	        { "YouTubeVideo", k_EItemPreviewType_YouTubeVideo },
	        { "Sketchfab", k_EItemPreviewType_Sketchfab },
	        { "EnvironmentMapHorizontalCross", k_EItemPreviewType_EnvironmentMap_HorizontalCross },
	        { "EnvironmentMapLatLong", k_EItemPreviewType_EnvironmentMap_LatLong },
	        { "Clip", k_EItemPreviewType_Clip },
	        { "ReservedMax", k_EItemPreviewType_ReservedMax },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "RemoteStoragePublishedFileVisibility",
	    {
	        { "Public", k_ERemoteStoragePublishedFileVisibilityPublic },
	        { "FriendsOnly", k_ERemoteStoragePublishedFileVisibilityFriendsOnly },
	        { "Private", k_ERemoteStoragePublishedFileVisibilityPrivate },
	        { "Unlisted", k_ERemoteStoragePublishedFileVisibilityUnlisted },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "WorkshopFileType",
	    {
	        { "Community", k_EWorkshopFileTypeCommunity },
	        { "Microtransaction", k_EWorkshopFileTypeMicrotransaction },
	        { "Collection", k_EWorkshopFileTypeCollection },
	        { "Art", k_EWorkshopFileTypeArt },
	        { "Video", k_EWorkshopFileTypeVideo },
	        { "Screenshot", k_EWorkshopFileTypeScreenshot },
	        { "Game", k_EWorkshopFileTypeGame },
	        { "Software", k_EWorkshopFileTypeSoftware },
	        { "Concept", k_EWorkshopFileTypeConcept },
	        { "WebGuide", k_EWorkshopFileTypeWebGuide },
	        { "IntegratedGuide", k_EWorkshopFileTypeIntegratedGuide },
	        { "Merch", k_EWorkshopFileTypeMerch },
	        { "ControllerBinding", k_EWorkshopFileTypeControllerBinding },
	        { "SteamworksAccessInvite", k_EWorkshopFileTypeSteamworksAccessInvite },
	        { "SteamVideo", k_EWorkshopFileTypeSteamVideo },
	        { "GameManagedItem", k_EWorkshopFileTypeGameManagedItem },
	        { "Clip", k_EWorkshopFileTypeClip },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "FloatingGamepadTextInputMode",
	    {
	        { "SingleLine", k_EFloatingGamepadTextInputModeModeSingleLine },
	        { "MultipleLines", k_EFloatingGamepadTextInputModeModeMultipleLines },
	        { "Email", k_EFloatingGamepadTextInputModeModeEmail },
	        { "Numeric", k_EFloatingGamepadTextInputModeModeNumeric },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "P2PSendType",
	    {
	        { "Unreliable", k_EP2PSendUnreliable },
	        { "UnreliableNoDelay", k_EP2PSendUnreliableNoDelay },
	        { "Reliable", k_EP2PSendReliable },
	        { "ReliableWithBuffering", k_EP2PSendReliableWithBuffering },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "BeginAuthSessionResult",
	    {
	        { "OK", k_EBeginAuthSessionResultOK },
	        { "InvalidTicket", k_EBeginAuthSessionResultInvalidTicket },
	        { "DuplicateRequest", k_EBeginAuthSessionResultDuplicateRequest },
	        { "InvalidVersion", k_EBeginAuthSessionResultInvalidVersion },
	        { "GameMismatch", k_EBeginAuthSessionResultGameMismatch },
	        { "ExpiredTicket", k_EBeginAuthSessionResultExpiredTicket },
	    }
	);

	setEnum(
	    env,
	    exports,
	    "AuthSessionResponse",
	    {
	        { "OK", k_EAuthSessionResponseOK },
	        { "UserNotConnectedToSteam", k_EAuthSessionResponseUserNotConnectedToSteam },
	        { "NoLicenseOrExpired", k_EAuthSessionResponseNoLicenseOrExpired },
	        { "VacBanned", k_EAuthSessionResponseVACBanned },
	        { "LoggedInElsewhere", k_EAuthSessionResponseLoggedInElseWhere },
	        { "VacCheckTimedOut", k_EAuthSessionResponseVACCheckTimedOut },
	        { "AuthTicketCanceled", k_EAuthSessionResponseAuthTicketCanceled },
	        { "AuthTicketInvalidAlreadyUsed", k_EAuthSessionResponseAuthTicketInvalidAlreadyUsed },
	        { "AuthTicketInvalid", k_EAuthSessionResponseAuthTicketInvalid },
	        { "PublisherIssuedBan", k_EAuthSessionResponsePublisherIssuedBan },
	        { "AuthTicketNetworkIdentityFailure", k_EAuthSessionResponseAuthTicketNetworkIdentityFailure },
	    }
	);
}
} // namespace steam_api::enums
