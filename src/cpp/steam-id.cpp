#include "steam-id.hpp"

namespace steam_api::steam_id {
CSteamID steamIdFromString(Napi::Env env, const std::string &source) {
	uint64 value = 0;
	if (!uint64FromJsString(env, source, "steamId", &value)) {
		return CSteamID();
	}
	return CSteamID(value);
}

bool requireSteamId(Napi::Env env, const Napi::CallbackInfo &info, CSteamID *target) {
	if (info.Length() < 1 || !info[0].IsString()) {
		JS_THROW("steamId must be a uint64 decimal string.");
		return false;
	}

	*target = steamIdFromString(env, info[0].As<Napi::String>().Utf8Value());
	return !env.IsExceptionPending();
}

ISteamFriends *steamFriends(Napi::Env env) {
	ISteamFriends *value = SteamFriends();
	if (value == nullptr) {
		JS_THROW("SteamFriends is not available. Call steam.initEx() first.");
	}
	return value;
}

JS_METHOD(isAnonymous) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BAnonAccount());
}

JS_METHOD(isAnonymousGameServer) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BAnonGameServerAccount());
}

JS_METHOD(isAnonymousGameServerLogin) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BBlankAnonAccount());
}

JS_METHOD(isAnonymousUser) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BAnonUserAccount());
}

JS_METHOD(isChatAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BChatAccount());
}

JS_METHOD(isClanAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BClanAccount());
}

JS_METHOD(isConsoleUserAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BConsoleUserAccount());
}

JS_METHOD(isContentServerAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BContentServerAccount());
}

JS_METHOD(isGameServerAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BGameServerAccount());
}

JS_METHOD(isIndividualAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BIndividualAccount());
}

JS_METHOD(isPersistentGameServerAccount) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.BPersistentGameServerAccount());
}

JS_METHOD(isLobby) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.IsLobby());
}

JS_METHOD(getAccountId) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_NUM(value.GetAccountID());
}

JS_METHOD(getRawSteamId) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_VALUE(jsStringFromUint64(env, value.ConvertToUint64()));
}

JS_METHOD(getAccountType) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_NUM(value.GetEAccountType());
}

JS_METHOD(isValid) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_BOOL(value.IsValid());
}

JS_METHOD(getStaticAccountKey) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}
	RET_VALUE(jsStringFromUint64(env, value.GetStaticAccountKey()));
}

JS_METHOD(getPersonaName) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}

	ISteamFriends *friends = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_STR(friends->GetFriendPersonaName(value));
}

JS_METHOD(getNickname) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}

	ISteamFriends *friends = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	const char *nickname = friends->GetPlayerNickname(value);
	RET_STR(nickname == nullptr ? "" : nickname);
}

JS_METHOD(getRelationship) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}

	ISteamFriends *friends = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(friends->GetFriendRelationship(value));
}

JS_METHOD(getSteamLevel) {
	NAPI_ENV;
	CSteamID value;
	if (!requireSteamId(env, info, &value)) {
		RET_UNDEFINED;
	}

	ISteamFriends *friends = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(friends->GetFriendSteamLevel(value));
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("isAnonymous", Napi::Function::New(env, isAnonymous));
	value.Set("isAnonymousGameServer", Napi::Function::New(env, isAnonymousGameServer));
	value.Set("isAnonymousGameServerLogin", Napi::Function::New(env, isAnonymousGameServerLogin));
	value.Set("isAnonymousUser", Napi::Function::New(env, isAnonymousUser));
	value.Set("isChatAccount", Napi::Function::New(env, isChatAccount));
	value.Set("isClanAccount", Napi::Function::New(env, isClanAccount));
	value.Set("isConsoleUserAccount", Napi::Function::New(env, isConsoleUserAccount));
	value.Set("isContentServerAccount", Napi::Function::New(env, isContentServerAccount));
	value.Set("isGameServerAccount", Napi::Function::New(env, isGameServerAccount));
	value.Set("isIndividualAccount", Napi::Function::New(env, isIndividualAccount));
	value.Set("isPersistentGameServerAccount", Napi::Function::New(env, isPersistentGameServerAccount));
	value.Set("isLobby", Napi::Function::New(env, isLobby));
	value.Set("getAccountId", Napi::Function::New(env, getAccountId));
	value.Set("getRawSteamId", Napi::Function::New(env, getRawSteamId));
	value.Set("getAccountType", Napi::Function::New(env, getAccountType));
	value.Set("isValid", Napi::Function::New(env, isValid));
	value.Set("getStaticAccountKey", Napi::Function::New(env, getStaticAccountKey));
	value.Set("getPersonaName", Napi::Function::New(env, getPersonaName));
	value.Set("getNickname", Napi::Function::New(env, getNickname));
	value.Set("getRelationship", Napi::Function::New(env, getRelationship));
	value.Set("getSteamLevel", Napi::Function::New(env, getSteamLevel));
	return value;
}
} // namespace steam_api::steam_id
