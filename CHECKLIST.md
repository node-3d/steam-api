# Greenworks Capability Checklist

## Lifecycle, Settings, Apps, and Utils

- [x] `greenworks.initAPI()`
- [x] `greenworks.init()`
- [x] `greenworks.restartAppIfNecessary(appId)`
- [x] `greenworks.isSteamRunning()`
- [x] `greenworks.isSteamRunningOnSteamDeck()`
- [x] `greenworks.getSteamId()`
- [x] `greenworks.getAppId()`
- [x] `greenworks.getAppBuildId()`
- [x] `greenworks.getCurrentGameLanguage()`
- [x] `greenworks.getCurrentUILanguage()`
- [x] `greenworks.getCurrentGameInstallDir()`
- [x] `greenworks.getAppInstallDir(appId, buffer, buffer_size)`
- [x] `greenworks.getNumberOfPlayers(success_callback, [error_callback])`
- [x] `greenworks.activateGameOverlay(option)`
- [x] `game-overlay-activated` event
- [x] `greenworks.activateGameOverlayToWebPage(url)`
- [x] `greenworks.activateGameOverlayToStore(appId, storeFlag)`
- [x] `greenworks.isGameOverlayEnabled()`
- [x] `greenworks.isSteamInBigPictureMode()`
- [x] `greenworks.isSubscribedApp(appId)`
- [x] `greenworks.isAppInstalled(appId)`
- [x] `greenworks.getImageSize(handle)`
- [x] `greenworks.getImageRGBA(handle)`
- [x] `greenworks.getIPCountry()`
- [x] `greenworks.getLaunchCommandLine()`
- [x] `steam-servers-connected` event
- [x] `steam-servers-disconnected` event
- [x] `steam-server-connect-failure` event
- [x] `steam-shutdown` event
- [x] `new-url-launch-parameters` event

## DLC

- [x] `greenworks.getDLCCount()`
- [x] `greenworks.getDLCDataByIndex(index)`
- [x] `greenworks.isDLCInstalled(dlc_app_id)`
- [x] `greenworks.installDLC(dlc_app_id)`
- [x] `greenworks.uninstallDLC(dlc_app_id)`
- [x] `dlc-installed` event

## Authentication

- [x] `greenworks.getAuthSessionTicket(success_callback, [error_callback])`
- [x] `greenworks.getAuthSessionTicketForWebAPI(identity, success_callback, [error_callback])`
- [x] `greenworks.cancelAuthTicket(ticket_handle)`
- [x] `greenworks.beginAuthSessionAsUser(ticket, steam_id)`
- [x] `greenworks.endAuthSessionAsUser(steam_id)`
- [x] `greenworks.getEncryptedAppTicket(user_data, success_callback, [error_callback])`
- [x] `greenworks.decryptAppTicket(encrypted_ticket, decryption_key)`
- [x] `greenworks.isTicketForApp(decrypted_ticket, app_id)`
- [x] `greenworks.getTicketIssueTime(decrypted_ticket)`
- [x] `greenworks.getTicketSteamId(decrypted_ticket)`
- [x] `greenworks.getTicketAppId(decrypted_ticket)`
- [x] `greenworks.EncryptedAppTicketSymmetricKeyLength`
- [x] `validate-auth-ticket` event

## Achievements and Stats

- [x] `greenworks.activateAchievement(achievement, success_callback, [error_callback])`
- [x] `greenworks.indicateAchievementProgress(achievement, current, max)`
- [x] `greenworks.getAchievement(achievement, success_callback, [error_callback])`
- [x] `greenworks.clearAchievement(achievement, success_callback, [error_callback])`
- [x] `greenworks.getAchievementNames()`
- [x] `greenworks.getNumberOfAchievements()`
- [x] `greenworks.getStatInt(name)`
- [x] `greenworks.getStatFloat(name)`
- [x] `greenworks.setStat(name, value)`
- [x] `greenworks.storeStats(success_callback, [error_callback])`
- [x] `greenworks.resetAllStats(achievementsToo)`

## Cloud and Remote Storage

- [x] `greenworks.saveTextToFile(file_name, file_content, success_callback, [error_callback])`
- [x] `greenworks.readTextFromFile(file_name, success_callback, [error_callback])`
- [x] `greenworks.deleteFile(file_name, success_callback, [error_callback])`
- [x] `greenworks.saveFilesToCloud(files_path, success_callback, [error_callback])`
- [x] `greenworks.isCloudEnabledForUser()`
- [x] `greenworks.isCloudEnabled()`
- [x] `greenworks.enableCloud(flag)`
- [x] `greenworks.getCloudQuota(success_callback, [error_callback])`
- [x] `greenworks.getFileCount()`
- [x] `greenworks.getFileNameAndSize(index)`

## Friends and SteamID

- [x] `SteamID.isAnonymous()`
- [x] `SteamID.isAnonymousGameServer()`
- [x] `SteamID.isAnonymousGameServerLogin()`
- [x] `SteamID.isAnonymousUser()`
- [x] `SteamID.isChatAccount()`
- [x] `SteamID.isClanAccount()`
- [x] `SteamID.isConsoleUserAccount()`
- [x] `SteamID.isContentServerAccount()`
- [x] `SteamID.isGameServerAccount()`
- [x] `SteamID.isIndividualAccount()`
- [x] `SteamID.isPersistentGameServerAccount()`
- [x] `SteamID.isLobby()`
- [x] `SteamID.getAccountID()`
- [x] `SteamID.getRawSteamID()`
- [x] `SteamID.getAccountType()`
- [x] `SteamID.isValid()`
- [x] `SteamID.getStaticAccountKey()`
- [x] `SteamID.getPersonaName()`
- [x] `SteamID.getNickname()`
- [x] `SteamID.getRelationship()`
- [x] `SteamID.getSteamLevel()`
- [x] `greenworks.getFriendCount(friend_flag)`
- [x] `greenworks.getFriends(friend_flag)`
- [x] `greenworks.requestUserInformation(raw_steam_id, require_name_only)`
- [x] `greenworks.getSmallFriendAvatar(raw_steam_id)`
- [x] `greenworks.getMediumFriendAvatar(raw_steam_id)`
- [x] `greenworks.getLargeFriendAvatar(raw_steam_id)`
- [x] `greenworks.setListenForFriendsMessage(intercept_enabled)`
- [x] `greenworks.replyToFriendMessage(raw_steam_id, message)`
- [x] `greenworks.getFriendMessage(raw_steam_id, message_id, maximum_message_size)`
- [x] `greenworks.getFriendPersonaName(raw_steam_id)`
- [x] `greenworks.setRichPresence(pchKey, pchValue)`
- [x] `greenworks.clearRichPresence()`
- [x] `greenworks.getFriendRichPresence(steamIDFriend, pchKey)`
- [x] `greenworks.setPlayedWith(steamIDUserPlayedWith)`
- [x] `greenworks.getFriendGamePlayed(steamIDFriend)`
- [x] `greenworks.activateGameOverlayInviteDialog(steamIDLobby)`
- [x] `greenworks.activateGameOverlayToUser(pchDialog, steamID)`
- [x] `persona-state-change` event
- [x] `avatar-image-loaded` event
- [x] `game-connected-friend-chat-message` event
- [x] `rich-presence-join-requested` event

## Workshop and UGC

- [x] `greenworks.fileShare(file_path, success_callback, [error_callback])`
- [x] `greenworks.publishWorkshopFile([options,] file_path, image_path, title, description, success_callback, [error_callback])`
- [x] `greenworks.updatePublishedWorkshopFile([options,] published_file_handle, file_path, image_path, title, description, success_callback, [error_callback])`
- [x] `greenworks.ugcPublish(file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])`
- [x] `greenworks.ugcPublishUpdate(published_file_handle, file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])`
- [x] `greenworks.ugcGetItems([options,] ugc_matching_type, ugc_query_type, success_callback, [error_callback])`
- [x] `greenworks.ugcGetUserItems([options,] ugc_matching_type, ugc_list_sort_order, ugc_list, success_callback, [error_callback])`
- [x] `greenworks.ugcDownloadItem(download_file_handle, download_dir, success_callback, [error_callback])`
- [x] `greenworks.ugcSynchronizeItems([options,] sync_dir, success_callback, [error_callback])`
- [x] `greenworks.ugcUnsubscribe(published_file_handle, success_callback, [error_callback])`
- [x] `greenworks.ugcShowOverlay([published_file_id])`
- [x] `greenworks.ugcGetItemState(published_file_id)`
- [x] `greenworks.ugcGetItemInstallInfo(published_file_id)`
- [x] `_publishWorkshopFile`
- [x] `_updatePublishedWorkshopFile`
- [x] `_ugcGetItems`
- [x] `_ugcGetUserItems`
- [x] `_ugcSynchronizeItems`
- [x] `SteamUGCDetails`

## Matchmaking and Lobbies

- [x] `greenworks.createLobby(lobbyType, maxMembers)`
- [x] `greenworks.deleteLobbyData(steamIDLobby, pchKey)`
- [x] `greenworks.getLobbyByIndex(iLobby)`
- [x] `greenworks.getLobbyData(steamIDLobby, pchKey)`
- [x] `greenworks.getLobbyMemberByIndex(steamIDLobby, iMember)`
- [x] `greenworks.getNumLobbyMembers(steamIDLobby)`
- [x] `greenworks.getLobbyOwner(steamIDLobby)`
- [x] `greenworks.inviteUserToLobby(steamIDLobby, steamIDInvitee)`
- [x] `greenworks.joinLobby(steamIDLobby)`
- [x] `greenworks.leaveLobby(steamIDLobby)`
- [x] `greenworks.setLobbyData(steamIDLobby, pchKey, pchValue)`
- [x] `greenworks.setLobbyJoinable(steamIDLobby, bLobbyJoinable)`
- [x] `greenworks.setLobbyOwner(steamIDLobby, steamIDNewOwner)`
- [x] `greenworks.setLobbyType(steamIDLobby, eLobbyType)`
- [x] `greenworks.requestLobbyList()`
- [x] `greenworks.getLobbyMemberLimit(steamIDLobby)`
- [x] `greenworks.setLobbyMemberLimit(steamIDLobby, limit)`
- [x] `greenworks.getLobbyMemberData(steamIDLobby, steamIDMember, pchKey)`
- [x] `greenworks.setLobbyMemberData(steamIDLobby, pchKey, pchValue)`
- [x] `greenworks.getLobbyDataCount(steamIDLobby)`
- [x] `greenworks.getLobbyDataByIndex(steamIDLobby, index)`
- [x] `greenworks.sendLobbyChatMsg(steamIDLobby, data)`
- [x] `greenworks.getLobbyChatEntry(steamIDLobby, chatID)`
- [x] `lobby-created` event
- [x] `lobby-data-update` event
- [x] `lobby-enter` event
- [x] `lobby-invite` event
- [x] `lobby-join-requested` event
- [x] `lobby-match-list` event
- [x] `lobby-chat-update` event
- [x] `lobby-chat-msg` event

## P2P Networking

- [x] `greenworks.sendP2PPacket(steamId, sendType, data, nChannel)`
- [x] `greenworks.isP2PPacketAvailable(nChannel)`
- [x] `greenworks.readP2PPacket(size, nChannel)`
- [x] `greenworks.acceptP2PSessionWithUser(steamId)`
- [x] `greenworks.getP2PSessionState(steamIDUser)`
- [x] `greenworks.closeP2PSessionWithUser(steamIDUser)`
- [x] `greenworks.closeP2PChannelWithUser(steamIDUser, nChannel)`
- [x] `greenworks.isBehindNAT()`
- [x] `p2p-session-request` event
- [x] `p2p-session-connect-fail` event

## Floating Gamepad Text Input

- [x] `greenworks.showFloatingGamepadTextInput(keyboardMode, x, y, width, height)`
- [x] `floating-gamepad-text-input-dismissed` event

## Constants and Enums

- [x] `EncryptedAppTicketSymmetricKeyLength`
- [x] `FriendFlags`
- [x] `FriendRelationship`
- [x] `PersonaChange`
- [x] `AccountType`
- [x] `ChatEntryType`
- [x] `ChatMemberStateChange`
- [x] `LobbyComparison`
- [x] `LobbyDistanceFilter`
- [x] `LobbyType`
- [x] `Result`
- [x] `UGCMatchingType`
- [x] `UGCQueryType`
- [x] `UserUGCList`
- [x] `UserUGCListSortOrder`
- [x] `UGCItemState`
- [x] `FloatingGamepadTextInputMode`
- [x] P2P send type enum
