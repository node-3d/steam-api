#include "matchmaking.hpp"

#include "call-result.hpp"

#include <algorithm>
#include <utility>

namespace steam_api::matchmaking {
ISteamMatchmaking *steamMatchmaking(Napi::Env env) {
	ISteamMatchmaking *value = SteamMatchmaking();
	if (value == nullptr) {
		JS_THROW("SteamMatchmaking is not available. Call steam.initEx() first.");
	}
	return value;
}

bool requireSteamId(
    Napi::Env env, const Napi::CallbackInfo &info, uint32 index, const char *name, CSteamID *target
) {
	if (info.Length() <= index || !info[index].IsString()) {
		JS_THROW(std::string(name) + " must be a uint64 decimal string.");
		return false;
	}

	uint64 value = 0;
	if (!uint64FromJsString(env, info[index].As<Napi::String>().Utf8Value(), name, &value)) {
		return false;
	}

	*target = CSteamID(value);
	if (!target->IsValid()) {
		JS_THROW(std::string(name) + " is invalid.");
		return false;
	}
	return true;
}

Napi::Value steamIdOrNull(Napi::Env env, const CSteamID &value) {
	if (!value.IsValid()) {
		return env.Null();
	}
	return jsStringFromUint64(env, value.ConvertToUint64());
}

class CreateLobbyRequest : public PendingCallResultRequest<CreateLobbyRequest, LobbyCreated_t> {
	using Base = PendingCallResultRequest<CreateLobbyRequest, LobbyCreated_t>;

  public:
	explicit CreateLobbyRequest(Napi::Env env) : Base(env) {}

	void onCompleted(LobbyCreated_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam lobby creation failed: Steam API IO failure.").Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("lobbyId", jsStringFromUint64(env(), result->m_ulSteamIDLobby));
		deferred().Resolve(value);
	}
};

class JoinLobbyRequest : public PendingCallResultRequest<JoinLobbyRequest, LobbyEnter_t> {
	using Base = PendingCallResultRequest<JoinLobbyRequest, LobbyEnter_t>;

  public:
	explicit JoinLobbyRequest(Napi::Env env) : Base(env) {}

	void onCompleted(LobbyEnter_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam lobby join failed: Steam API IO failure.").Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("lobbyId", jsStringFromUint64(env(), result->m_ulSteamIDLobby));
		value.Set("chatPermissions", result->m_rgfChatPermissions);
		value.Set("locked", result->m_bLocked);
		value.Set("response", result->m_EChatRoomEnterResponse);
		deferred().Resolve(value);
	}
};

class LobbyListRequest : public PendingCallResultRequest<LobbyListRequest, LobbyMatchList_t> {
	using Base = PendingCallResultRequest<LobbyListRequest, LobbyMatchList_t>;

  public:
	explicit LobbyListRequest(Napi::Env env) : Base(env) {}

	void onCompleted(LobbyMatchList_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam lobby list request failed: Steam API IO failure.").Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("lobbiesMatching", result->m_nLobbiesMatching);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<CreateLobbyRequest>> pendingLobbyCreates;
std::vector<std::unique_ptr<JoinLobbyRequest>> pendingLobbyJoins;
std::vector<std::unique_ptr<LobbyListRequest>> pendingLobbyLists;

void rejectPendingPromises(const std::string &message) {
	rejectPendingCallResults(pendingLobbyCreates, message);
	rejectPendingCallResults(pendingLobbyJoins, message);
	rejectPendingCallResults(pendingLobbyLists, message);
}

void clearPendingPromises() {
	clearPendingCallResults(pendingLobbyCreates);
	clearPendingCallResults(pendingLobbyJoins);
	clearPendingCallResults(pendingLobbyLists);
}

JS_METHOD(createLobby) {
	NAPI_ENV;
	REQ_INT32_ARG(0, lobbyType);
	REQ_INT32_ARG(1, maxMembers);

	if (maxMembers <= 0) {
		JS_THROW("maxMembers must be greater than zero.");
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->CreateLobby(static_cast<ELobbyType>(lobbyType), maxMembers);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam lobby creation request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackCallResult<CreateLobbyRequest>(env, call, pendingLobbyCreates));
}

JS_METHOD(deleteLobbyData) {
	NAPI_ENV;
	REQ_STR_ARG(1, key);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->DeleteLobbyData(lobbyId, key.c_str()));
}

JS_METHOD(getLobbyByIndex) {
	NAPI_ENV;
	REQ_INT32_ARG(0, index);

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_VALUE(steamIdOrNull(env, value->GetLobbyByIndex(index)));
}

JS_METHOD(getLobbyData) {
	NAPI_ENV;
	REQ_STR_ARG(1, key);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_STR(value->GetLobbyData(lobbyId, key.c_str()));
}

JS_METHOD(getLobbyMemberByIndex) {
	NAPI_ENV;
	REQ_INT32_ARG(1, index);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_VALUE(steamIdOrNull(env, value->GetLobbyMemberByIndex(lobbyId, index)));
}

JS_METHOD(getNumLobbyMembers) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetNumLobbyMembers(lobbyId));
}

JS_METHOD(getLobbyOwner) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_VALUE(steamIdOrNull(env, value->GetLobbyOwner(lobbyId)));
}

JS_METHOD(inviteUserToLobby) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	CSteamID inviteeId;
	if (!requireSteamId(env, info, 1, "inviteeId", &inviteeId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->InviteUserToLobby(lobbyId, inviteeId));
}

JS_METHOD(joinLobby) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->JoinLobby(lobbyId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam lobby join request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackCallResult<JoinLobbyRequest>(env, call, pendingLobbyJoins));
}

JS_METHOD(leaveLobby) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->LeaveLobby(lobbyId);
	RET_UNDEFINED;
}

JS_METHOD(setLobbyData) {
	NAPI_ENV;
	REQ_STR_ARG(1, key);
	REQ_STR_ARG(2, data);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetLobbyData(lobbyId, key.c_str(), data.c_str()));
}

JS_METHOD(setLobbyJoinable) {
	NAPI_ENV;
	REQ_BOOL_ARG(1, joinable);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetLobbyJoinable(lobbyId, joinable));
}

JS_METHOD(setLobbyOwner) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	CSteamID ownerId;
	if (!requireSteamId(env, info, 1, "ownerId", &ownerId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetLobbyOwner(lobbyId, ownerId));
}

JS_METHOD(setLobbyType) {
	NAPI_ENV;
	REQ_INT32_ARG(1, lobbyType);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetLobbyType(lobbyId, static_cast<ELobbyType>(lobbyType)));
}

JS_METHOD(requestLobbyList) {
	NAPI_ENV;
	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->RequestLobbyList();
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam lobby list request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackCallResult<LobbyListRequest>(env, call, pendingLobbyLists));
}

JS_METHOD(getLobbyMemberLimit) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetLobbyMemberLimit(lobbyId));
}

JS_METHOD(setLobbyMemberLimit) {
	NAPI_ENV;
	REQ_INT32_ARG(1, limit);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetLobbyMemberLimit(lobbyId, limit));
}

JS_METHOD(getLobbyMemberData) {
	NAPI_ENV;
	REQ_STR_ARG(2, key);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	CSteamID memberId;
	if (!requireSteamId(env, info, 1, "memberId", &memberId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_STR(value->GetLobbyMemberData(lobbyId, memberId, key.c_str()));
}

JS_METHOD(setLobbyMemberData) {
	NAPI_ENV;
	REQ_STR_ARG(1, key);
	REQ_STR_ARG(2, data);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->SetLobbyMemberData(lobbyId, key.c_str(), data.c_str());
	RET_UNDEFINED;
}

JS_METHOD(getLobbyDataCount) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetLobbyDataCount(lobbyId));
}

JS_METHOD(getLobbyDataByIndex) {
	NAPI_ENV;
	REQ_INT32_ARG(1, index);

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<char> key(k_nMaxLobbyKeyLength + 1, '\0');
	std::vector<char> data(4096, '\0');
	bool ok = value->GetLobbyDataByIndex(
	    lobbyId, index, key.data(), static_cast<int>(key.size()), data.data(), static_cast<int>(data.size())
	);
	if (!ok) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("key", key.data());
	result.Set("value", data.data());
	RET_VALUE(result);
}

JS_METHOD(sendLobbyChatMsg) {
	NAPI_ENV;
	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	if (info.Length() <= 1 || !info[1].IsBuffer()) {
		JS_THROW("data must be a Buffer.");
		RET_UNDEFINED;
	}

	Napi::Buffer<char> data = info[1].As<Napi::Buffer<char>>();
	if (data.Length() > 4096) {
		JS_THROW("data must be at most 4096 bytes.");
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SendLobbyChatMsg(lobbyId, data.Data(), static_cast<int>(data.Length())));
}

JS_METHOD(getLobbyChatEntry) {
	NAPI_ENV;
	REQ_INT32_ARG(1, chatId);
	USE_INT32_ARG(2, maximumMessageSize, 4096);

	if (maximumMessageSize <= 0 || maximumMessageSize > 4096) {
		JS_THROW("maximumMessageSize must be between 1 and 4096.");
		RET_UNDEFINED;
	}

	CSteamID lobbyId;
	if (!requireSteamId(env, info, 0, "lobbyId", &lobbyId)) {
		RET_UNDEFINED;
	}

	ISteamMatchmaking *value = steamMatchmaking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	CSteamID userId;
	std::vector<char> data(static_cast<size_t>(maximumMessageSize));
	EChatEntryType chatEntryType = k_EChatEntryTypeInvalid;
	int bytes =
	    value->GetLobbyChatEntry(lobbyId, chatId, &userId, data.data(), maximumMessageSize, &chatEntryType);
	if (bytes < 0) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("steamIdUser", jsStringFromUint64(env, userId.ConvertToUint64()));
	result.Set("data", Napi::Buffer<char>::Copy(env, data.data(), static_cast<size_t>(bytes)));
	result.Set("chatEntryType", static_cast<int32_t>(chatEntryType));
	RET_VALUE(result);
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("createLobby", Napi::Function::New(env, createLobby));
	value.Set("deleteLobbyData", Napi::Function::New(env, deleteLobbyData));
	value.Set("getLobbyByIndex", Napi::Function::New(env, getLobbyByIndex));
	value.Set("getLobbyData", Napi::Function::New(env, getLobbyData));
	value.Set("getLobbyMemberByIndex", Napi::Function::New(env, getLobbyMemberByIndex));
	value.Set("getNumLobbyMembers", Napi::Function::New(env, getNumLobbyMembers));
	value.Set("getLobbyOwner", Napi::Function::New(env, getLobbyOwner));
	value.Set("inviteUserToLobby", Napi::Function::New(env, inviteUserToLobby));
	value.Set("joinLobby", Napi::Function::New(env, joinLobby));
	value.Set("leaveLobby", Napi::Function::New(env, leaveLobby));
	value.Set("setLobbyData", Napi::Function::New(env, setLobbyData));
	value.Set("setLobbyJoinable", Napi::Function::New(env, setLobbyJoinable));
	value.Set("setLobbyOwner", Napi::Function::New(env, setLobbyOwner));
	value.Set("setLobbyType", Napi::Function::New(env, setLobbyType));
	value.Set("requestLobbyList", Napi::Function::New(env, requestLobbyList));
	value.Set("getLobbyMemberLimit", Napi::Function::New(env, getLobbyMemberLimit));
	value.Set("setLobbyMemberLimit", Napi::Function::New(env, setLobbyMemberLimit));
	value.Set("getLobbyMemberData", Napi::Function::New(env, getLobbyMemberData));
	value.Set("setLobbyMemberData", Napi::Function::New(env, setLobbyMemberData));
	value.Set("getLobbyDataCount", Napi::Function::New(env, getLobbyDataCount));
	value.Set("getLobbyDataByIndex", Napi::Function::New(env, getLobbyDataByIndex));
	value.Set("sendLobbyChatMsg", Napi::Function::New(env, sendLobbyChatMsg));
	value.Set("getLobbyChatEntry", Napi::Function::New(env, getLobbyChatEntry));
	return value;
}
} // namespace steam_api::matchmaking
