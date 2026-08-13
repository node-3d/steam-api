#include "networking.hpp"

#include <algorithm>
#include <limits>

namespace steam_api::networking {
namespace {
constexpr uint32 kMaxP2PPacketReadSize = 1024 * 1024;
}

ISteamNetworking *steamNetworking(Napi::Env env) {
	ISteamNetworking *value = SteamNetworking();
	if (value == nullptr) {
		JS_THROW("SteamNetworking is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamUser *steamUser(Napi::Env env) {
	ISteamUser *value = SteamUser();
	if (value == nullptr) {
		JS_THROW("SteamUser is not available. Call steam.initEx() first.");
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

JS_METHOD(sendP2PPacket) {
	NAPI_ENV;
	REQ_INT32_ARG(1, sendType);

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	if (info.Length() <= 2 || !info[2].IsBuffer()) {
		JS_THROW("data must be a Buffer.");
		RET_UNDEFINED;
	}

	Napi::Buffer<char> data = info[2].As<Napi::Buffer<char>>();
	if (data.Length() > static_cast<size_t>(std::numeric_limits<uint32>::max())) {
		JS_THROW("data is too large for Steam P2P packets.");
		RET_UNDEFINED;
	}

	USE_INT32_ARG(3, channel, 0);

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SendP2PPacket(
	    steamId, data.Data(), static_cast<uint32>(data.Length()), static_cast<EP2PSend>(sendType), channel
	));
}

JS_METHOD(isP2PPacketAvailable) {
	NAPI_ENV;
	USE_INT32_ARG(0, channel, 0);

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 messageSize = 0;
	if (!value->IsP2PPacketAvailable(&messageSize, channel)) {
		RET_NUM(0);
	}
	RET_NUM(messageSize);
}

JS_METHOD(readP2PPacket) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, size);
	if (size > kMaxP2PPacketReadSize) {
		JS_THROW("size exceeds the maximum Steam P2P packet read size.");
		RET_UNDEFINED;
	}

	USE_INT32_ARG(1, channel, 0);

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<char> data(size);
	uint32 messageSize = 0;
	CSteamID steamIdRemote;
	bool ok = value->ReadP2PPacket(data.data(), size, &messageSize, &steamIdRemote, channel);
	if (!ok) {
		RET_NULL;
	}

	size_t bytesToCopy = std::min(static_cast<size_t>(messageSize), data.size());

	Napi::Object result = JS_OBJECT;
	result.Set("data", Napi::Buffer<char>::Copy(env, data.data(), bytesToCopy));
	result.Set("steamIdRemote", jsStringFromUint64(env, steamIdRemote.ConvertToUint64()));
	result.Set("messageSize", messageSize);
	RET_VALUE(result);
}

JS_METHOD(acceptP2PSessionWithUser) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->AcceptP2PSessionWithUser(steamId));
}

JS_METHOD(getP2PSessionState) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	P2PSessionState_t state = {};
	bool ok = value->GetP2PSessionState(steamId, &state);

	Napi::Object connectionState = JS_OBJECT;
	connectionState.Set("connectionActive", state.m_bConnectionActive != 0);
	connectionState.Set("connecting", state.m_bConnecting != 0);
	connectionState.Set("sessionError", state.m_eP2PSessionError);
	connectionState.Set("usingRelay", state.m_bUsingRelay != 0);
	connectionState.Set("bytesQueuedForSend", state.m_nBytesQueuedForSend);
	connectionState.Set("packetsQueuedForSend", state.m_nPacketsQueuedForSend);
	connectionState.Set("remoteIp", state.m_nRemoteIP);
	connectionState.Set("remotePort", state.m_nRemotePort);

	Napi::Object result = JS_OBJECT;
	result.Set("result", ok);
	result.Set("connectionState", connectionState);
	RET_VALUE(result);
}

JS_METHOD(closeP2PSessionWithUser) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->CloseP2PSessionWithUser(steamId));
}

JS_METHOD(closeP2PChannelWithUser) {
	NAPI_ENV;
	REQ_INT32_ARG(1, channel);

	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamNetworking *value = steamNetworking(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->CloseP2PChannelWithUser(steamId, channel));
}

JS_METHOD(isBehindNat) {
	NAPI_ENV;
	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BIsBehindNAT());
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("sendP2PPacket", Napi::Function::New(env, sendP2PPacket));
	value.Set("isP2PPacketAvailable", Napi::Function::New(env, isP2PPacketAvailable));
	value.Set("readP2PPacket", Napi::Function::New(env, readP2PPacket));
	value.Set("acceptP2PSessionWithUser", Napi::Function::New(env, acceptP2PSessionWithUser));
	value.Set("getP2PSessionState", Napi::Function::New(env, getP2PSessionState));
	value.Set("closeP2PSessionWithUser", Napi::Function::New(env, closeP2PSessionWithUser));
	value.Set("closeP2PChannelWithUser", Napi::Function::New(env, closeP2PChannelWithUser));
	value.Set("isBehindNat", Napi::Function::New(env, isBehindNat));
	return value;
}
} // namespace steam_api::networking
