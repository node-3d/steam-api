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
- [ ] `greenworks.getNumberOfPlayers(success_callback, [error_callback])`
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
- [ ] `greenworks.getAuthSessionTicketForWebAPI(identity, success_callback, [error_callback])`
- [x] `greenworks.cancelAuthTicket(ticket_handle)`
- [ ] `greenworks.beginAuthSessionAsUser(ticket, steam_id)`
- [ ] `greenworks.endAuthSessionAsUser(steam_id)`
- [ ] `greenworks.getEncryptedAppTicket(user_data, success_callback, [error_callback])`
- [ ] `greenworks.decryptAppTicket(encrypted_ticket, decryption_key)`
- [ ] `greenworks.isTicketForApp(decrypted_ticket, app_id)`
- [ ] `greenworks.getTicketIssueTime(decrypted_ticket)`
- [ ] `greenworks.getTicketSteamId(decrypted_ticket)`
- [ ] `greenworks.getTicketAppId(decrypted_ticket)`
- [ ] `greenworks.EncryptedAppTicketSymmetricKeyLength`
- [ ] `validate-auth-ticket` event

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

- [ ] `greenworks.saveTextToFile(file_name, file_content, success_callback, [error_callback])`
- [ ] `greenworks.readTextFromFile(file_name, success_callback, [error_callback])`
- [ ] `greenworks.deleteFile(file_name, success_callback, [error_callback])`
- [ ] `greenworks.saveFilesToCloud(files_path, success_callback, [error_callback])`
- [ ] `greenworks.isCloudEnabledForUser()`
- [ ] `greenworks.isCloudEnabled()`
- [ ] `greenworks.enableCloud(flag)`
- [ ] `greenworks.getCloudQuota(success_callback, [error_callback])`
- [ ] `greenworks.getFileCount()`
- [ ] `greenworks.getFileNameAndSize(index)`

## Friends and SteamID

- [ ] `SteamID.isAnonymous()`
- [ ] `SteamID.isAnonymousGameServer()`
- [ ] `SteamID.isAnonymousGameServerLogin()`
- [ ] `SteamID.isAnonymousUser()`
- [ ] `SteamID.isChatAccount()`
- [ ] `SteamID.isClanAccount()`
- [ ] `SteamID.isConsoleUserAccount()`
- [ ] `SteamID.isContentServerAccount()`
- [ ] `SteamID.isGameServerAccount()`
- [ ] `SteamID.isIndividualAccount()`
- [ ] `SteamID.isPersistentGameServerAccount()`
- [ ] `SteamID.isLobby()`
- [ ] `SteamID.getAccountID()`
- [ ] `SteamID.getRawSteamID()`
- [ ] `SteamID.getAccountType()`
- [ ] `SteamID.isValid()`
- [ ] `SteamID.getStaticAccountKey()`
- [ ] `SteamID.getPersonaName()`
- [ ] `SteamID.getNickname()`
- [ ] `SteamID.getRelationship()`
- [ ] `SteamID.getSteamLevel()`
- [ ] `greenworks.getFriendCount(friend_flag)`
- [ ] `greenworks.getFriends(friend_flag)`
- [ ] `greenworks.requestUserInformation(raw_steam_id, require_name_only)`
- [ ] `greenworks.getSmallFriendAvatar(raw_steam_id)`
- [ ] `greenworks.getMediumFriendAvatar(raw_steam_id)`
- [ ] `greenworks.getLargeFriendAvatar(raw_steam_id)`
- [ ] `greenworks.setListenForFriendsMessage(intercept_enabled)`
- [ ] `greenworks.replyToFriendMessage(raw_steam_id, message)`
- [ ] `greenworks.getFriendMessage(raw_steam_id, message_id, maximum_message_size)`
- [ ] `greenworks.getFriendPersonaName(raw_steam_id)`
- [ ] `greenworks.setRichPresence(pchKey, pchValue)`
- [ ] `greenworks.clearRichPresence()`
- [ ] `greenworks.getFriendRichPresence(steamIDFriend, pchKey)`
- [ ] `greenworks.setPlayedWith(steamIDUserPlayedWith)`
- [ ] `greenworks.getFriendGamePlayed(steamIDFriend)`
- [ ] `greenworks.activateGameOverlayInviteDialog(steamIDLobby)`
- [ ] `greenworks.activateGameOverlayToUser(pchDialog, steamID)`
- [ ] `persona-state-change` event
- [ ] `avatar-image-loaded` event
- [ ] `game-connected-friend-chat-message` event
- [ ] `rich-presence-join-requested` event

## Workshop and UGC

- [ ] `greenworks.fileShare(file_path, success_callback, [error_callback])`
- [ ] `greenworks.publishWorkshopFile([options,] file_path, image_path, title, description, success_callback, [error_callback])`
- [ ] `greenworks.updatePublishedWorkshopFile([options,] published_file_handle, file_path, image_path, title, description, success_callback, [error_callback])`
- [ ] `greenworks.ugcPublish(file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])`
- [ ] `greenworks.ugcPublishUpdate(published_file_handle, file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])`
- [ ] `greenworks.ugcGetItems([options,] ugc_matching_type, ugc_query_type, success_callback, [error_callback])`
- [ ] `greenworks.ugcGetUserItems([options,] ugc_matching_type, ugc_list_sort_order, ugc_list, success_callback, [error_callback])`
- [ ] `greenworks.ugcDownloadItem(download_file_handle, download_dir, success_callback, [error_callback])`
- [ ] `greenworks.ugcSynchronizeItems([options,] sync_dir, success_callback, [error_callback])`
- [ ] `greenworks.ugcUnsubscribe(published_file_handle, success_callback, [error_callback])`
- [ ] `greenworks.ugcShowOverlay([published_file_id])`
- [ ] `greenworks.ugcGetItemState(published_file_id)`
- [ ] `greenworks.ugcGetItemInstallInfo(published_file_id)`
- [ ] `_publishWorkshopFile`
- [ ] `_updatePublishedWorkshopFile`
- [ ] `_ugcGetItems`
- [ ] `_ugcGetUserItems`
- [ ] `_ugcSynchronizeItems`
- [ ] `SteamUGCDetails`

## Matchmaking and Lobbies

- [ ] `greenworks.createLobby(lobbyType, maxMembers)`
- [ ] `greenworks.deleteLobbyData(steamIDLobby, pchKey)`
- [ ] `greenworks.getLobbyByIndex(iLobby)`
- [ ] `greenworks.getLobbyData(steamIDLobby, pchKey)`
- [ ] `greenworks.getLobbyMemberByIndex(steamIDLobby, iMember)`
- [ ] `greenworks.getNumLobbyMembers(steamIDLobby)`
- [ ] `greenworks.getLobbyOwner(steamIDLobby)`
- [ ] `greenworks.inviteUserToLobby(steamIDLobby, steamIDInvitee)`
- [ ] `greenworks.joinLobby(steamIDLobby)`
- [ ] `greenworks.leaveLobby(steamIDLobby)`
- [ ] `greenworks.setLobbyData(steamIDLobby, pchKey, pchValue)`
- [ ] `greenworks.setLobbyJoinable(steamIDLobby, bLobbyJoinable)`
- [ ] `greenworks.setLobbyOwner(steamIDLobby, steamIDNewOwner)`
- [ ] `greenworks.setLobbyType(steamIDLobby, eLobbyType)`
- [ ] `greenworks.requestLobbyList()`
- [ ] `greenworks.getLobbyMemberLimit(steamIDLobby)`
- [ ] `greenworks.setLobbyMemberLimit(steamIDLobby, limit)`
- [ ] `greenworks.getLobbyMemberData(steamIDLobby, steamIDMember, pchKey)`
- [ ] `greenworks.setLobbyMemberData(steamIDLobby, pchKey, pchValue)`
- [ ] `greenworks.getLobbyDataCount(steamIDLobby)`
- [ ] `greenworks.getLobbyDataByIndex(steamIDLobby, index)`
- [ ] `greenworks.sendLobbyChatMsg(steamIDLobby, data)`
- [ ] `greenworks.getLobbyChatEntry(steamIDLobby, chatID)`
- [ ] `lobby-created` event
- [ ] `lobby-data-update` event
- [ ] `lobby-enter` event
- [ ] `lobby-invite` event
- [ ] `lobby-join-requested` event
- [ ] `lobby-match-list` event
- [ ] `lobby-chat-update` event
- [ ] `lobby-chat-msg` event

## P2P Networking

- [ ] `greenworks.sendP2PPacket(steamId, sendType, data, nChannel)`
- [ ] `greenworks.isP2PPacketAvailable(nChannel)`
- [ ] `greenworks.readP2PPacket(size, nChannel)`
- [ ] `greenworks.acceptP2PSessionWithUser(steamId)`
- [ ] `greenworks.getP2PSessionState(steamIDUser)`
- [ ] `greenworks.closeP2PSessionWithUser(steamIDUser)`
- [ ] `greenworks.closeP2PChannelWithUser(steamIDUser, nChannel)`
- [ ] `greenworks.isBehindNAT()`
- [ ] `p2p-session-request` event
- [ ] `p2p-session-connect-fail` event

## Archive Utilities

- [ ] `greenworks.Utils.move(source_dir, target_dir, [success_callback], [error_callback])`
- [ ] `greenworks.Utils.createArchive(zip_file_path, source_dir, password, compress_level, success_callback, [error_callback])`
- [ ] `greenworks.Utils.extractArchive(zip_file_path, extract_dir, password, success_callback, [error_callback])`

## Floating Gamepad Text Input

- [ ] `greenworks.showFloatingGamepadTextInput(keyboardMode, x, y, width, height)`
- [x] `floating-gamepad-text-input-dismissed` event

## Constants and Enums

- [ ] `_version`
- [ ] `EncryptedAppTicketSymmetricKeyLength`
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
