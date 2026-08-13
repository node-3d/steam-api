#include "friends.hpp"

#include <algorithm>

namespace steam_api::friends {
namespace {
constexpr int32 kMaxFriendMessageSize = 64 * 1024;
}

ISteamFriends *steamFriends(Napi::Env env) {
	ISteamFriends *value = SteamFriends();
	if (value == nullptr) {
		JS_THROW("SteamFriends is not available. Call steam.initEx() first.");
	}
	return value;
}

bool requireSteamId(Napi::Env env, const Napi::CallbackInfo &info, uint32 index, CSteamID *target) {
	if (info.Length() <= index || !info[index].IsString()) {
		JS_THROW("steamId must be a uint64 decimal string.");
		return false;
	}

	uint64 value = 0;
	if (!uint64FromJsString(env, info[index].As<Napi::String>().Utf8Value(), "steamId", &value)) {
		return false;
	}

	*target = CSteamID(value);
	if (!target->IsValid()) {
		JS_THROW("steamId is invalid.");
		return false;
	}
	return true;
}

JS_METHOD(activateGameOverlay) {
	NAPI_ENV;
	REQ_STR_ARG(0, dialog);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlay(dialog.c_str());
	RET_UNDEFINED;
}

JS_METHOD(activateGameOverlayToWebPage) {
	NAPI_ENV;
	REQ_STR_ARG(0, url);
	USE_INT32_ARG(1, mode, k_EActivateGameOverlayToWebPageMode_Default);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayToWebPage(url.c_str(), static_cast<EActivateGameOverlayToWebPageMode>(mode));
	RET_UNDEFINED;
}

JS_METHOD(activateGameOverlayToStore) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	REQ_INT32_ARG(1, flag);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayToStore(appId, static_cast<EOverlayToStoreFlag>(flag));
	RET_UNDEFINED;
}

JS_METHOD(getFriendCount) {
	NAPI_ENV;
	REQ_INT32_ARG(0, friendFlag);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetFriendCount(static_cast<EFriendFlags>(friendFlag)));
}

JS_METHOD(getFriends) {
	NAPI_ENV;
	REQ_INT32_ARG(0, friendFlag);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	EFriendFlags flags = static_cast<EFriendFlags>(friendFlag);
	int count = value->GetFriendCount(flags);
	Napi::Array result = Napi::Array::New(env, count < 0 ? 0 : static_cast<uint32>(count));
	for (int i = 0; i < count; i++) {
		result.Set(
		    static_cast<uint32>(i),
		    jsStringFromUint64(env, value->GetFriendByIndex(i, flags).ConvertToUint64())
		);
	}
	RET_VALUE(result);
}

JS_METHOD(requestUserInformation) {
	NAPI_ENV;
	REQ_BOOL_ARG(1, requireNameOnly);

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->RequestUserInformation(steamId, requireNameOnly));
}

JS_METHOD(getSmallFriendAvatar) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetSmallFriendAvatar(steamId));
}

JS_METHOD(getMediumFriendAvatar) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetMediumFriendAvatar(steamId));
}

JS_METHOD(getLargeFriendAvatar) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetLargeFriendAvatar(steamId));
}

JS_METHOD(setListenForFriendsMessage) {
	NAPI_ENV;
	REQ_BOOL_ARG(0, interceptEnabled);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetListenForFriendsMessages(interceptEnabled));
}

JS_METHOD(replyToFriendMessage) {
	NAPI_ENV;
	REQ_STR_ARG(1, message);

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->ReplyToFriendMessage(steamId, message.c_str()));
}

JS_METHOD(getFriendMessage) {
	NAPI_ENV;
	REQ_INT32_ARG(1, messageId);
	REQ_INT32_ARG(2, maximumMessageSize);

	if (maximumMessageSize <= 0) {
		JS_THROW("maximumMessageSize must be greater than zero.");
		RET_UNDEFINED;
	}
	if (maximumMessageSize > kMaxFriendMessageSize) {
		JS_THROW("maximumMessageSize exceeds the maximum Steam friend message size.");
		RET_UNDEFINED;
	}

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<char> message(static_cast<size_t>(maximumMessageSize));
	EChatEntryType chatEntryType = k_EChatEntryTypeInvalid;
	int messageSize =
	    value->GetFriendMessage(steamId, messageId, message.data(), maximumMessageSize, &chatEntryType);
	if (messageSize < 0) {
		RET_NULL;
	}

	size_t bytesToCopy = std::min(static_cast<size_t>(messageSize), message.size());

	Napi::Object result = JS_OBJECT;
	result.Set("message", Napi::String::New(env, message.data(), bytesToCopy));
	result.Set("chatEntryType", static_cast<int32_t>(chatEntryType));
	RET_VALUE(result);
}

JS_METHOD(getFriendPersonaName) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_STR(value->GetFriendPersonaName(steamId));
}

JS_METHOD(setRichPresence) {
	NAPI_ENV;
	REQ_STR_ARG(0, key);
	REQ_STR_ARG(1, richPresence);

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetRichPresence(key.c_str(), richPresence.c_str()));
}

JS_METHOD(clearRichPresence) {
	NAPI_ENV;
	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ClearRichPresence();
	RET_UNDEFINED;
}

JS_METHOD(getFriendRichPresence) {
	NAPI_ENV;
	REQ_STR_ARG(1, key);

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_STR(value->GetFriendRichPresence(steamId, key.c_str()));
}

JS_METHOD(setPlayedWith) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->SetPlayedWith(steamId);
	RET_UNDEFINED;
}

JS_METHOD(getFriendGamePlayed) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	FriendGameInfo_t gameInfo = {};
	if (!value->GetFriendGamePlayed(steamId, &gameInfo)) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("gameId", jsStringFromUint64(env, gameInfo.m_gameID.ToUint64()));
	result.Set("gameIp", gameInfo.m_unGameIP);
	result.Set("gamePort", gameInfo.m_usGamePort);
	result.Set("queryPort", gameInfo.m_usQueryPort);
	result.Set("steamIdLobby", jsStringFromUint64(env, gameInfo.m_steamIDLobby.ConvertToUint64()));
	RET_VALUE(result);
}

JS_METHOD(activateGameOverlayInviteDialog) {
	NAPI_ENV;
	CSteamID steamIdLobby;
	if (!requireSteamId(env, info, 0, &steamIdLobby)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayInviteDialog(steamIdLobby);
	RET_UNDEFINED;
}

JS_METHOD(activateGameOverlayToUser) {
	NAPI_ENV;
	REQ_STR_ARG(0, dialog);

	CSteamID steamId;
	if (!requireSteamId(env, info, 1, &steamId)) {
		RET_UNDEFINED;
	}

	ISteamFriends *value = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->ActivateGameOverlayToUser(dialog.c_str(), steamId);
	RET_UNDEFINED;
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("activateGameOverlay", Napi::Function::New(env, activateGameOverlay));
	value.Set("activateGameOverlayToWebPage", Napi::Function::New(env, activateGameOverlayToWebPage));
	value.Set("activateGameOverlayToStore", Napi::Function::New(env, activateGameOverlayToStore));
	value.Set("getFriendCount", Napi::Function::New(env, getFriendCount));
	value.Set("getFriends", Napi::Function::New(env, getFriends));
	value.Set("requestUserInformation", Napi::Function::New(env, requestUserInformation));
	value.Set("getSmallFriendAvatar", Napi::Function::New(env, getSmallFriendAvatar));
	value.Set("getMediumFriendAvatar", Napi::Function::New(env, getMediumFriendAvatar));
	value.Set("getLargeFriendAvatar", Napi::Function::New(env, getLargeFriendAvatar));
	value.Set("setListenForFriendsMessage", Napi::Function::New(env, setListenForFriendsMessage));
	value.Set("replyToFriendMessage", Napi::Function::New(env, replyToFriendMessage));
	value.Set("getFriendMessage", Napi::Function::New(env, getFriendMessage));
	value.Set("getFriendPersonaName", Napi::Function::New(env, getFriendPersonaName));
	value.Set("setRichPresence", Napi::Function::New(env, setRichPresence));
	value.Set("clearRichPresence", Napi::Function::New(env, clearRichPresence));
	value.Set("getFriendRichPresence", Napi::Function::New(env, getFriendRichPresence));
	value.Set("setPlayedWith", Napi::Function::New(env, setPlayedWith));
	value.Set("getFriendGamePlayed", Napi::Function::New(env, getFriendGamePlayed));
	value.Set("activateGameOverlayInviteDialog", Napi::Function::New(env, activateGameOverlayInviteDialog));
	value.Set("activateGameOverlayToUser", Napi::Function::New(env, activateGameOverlayToUser));
	return value;
}
} // namespace steam_api::friends
