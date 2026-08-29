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

ISteamNetworkingSockets *steamNetworkingSockets(Napi::Env env) {
	ISteamNetworkingSockets *value = SteamNetworkingSockets();
	if (value == nullptr) {
		JS_THROW("SteamNetworkingSockets is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamNetworkingMessages *steamNetworkingMessages(Napi::Env env) {
	ISteamNetworkingMessages *value = SteamNetworkingMessages();
	if (value == nullptr) {
		JS_THROW("SteamNetworkingMessages is not available. Call steam.initEx() first.");
	}
	return value;
}

bool requireIdentity(
    Napi::Env env,
    const Napi::CallbackInfo &info,
    uint32 index,
    const char *name,
    SteamNetworkingIdentity *target
) {
	CSteamID steamId;
	if (!requireSteamId(env, info, index, name, &steamId))
		return false;
	target->SetSteamID(steamId);
	return true;
}

Napi::Array copyAndReleaseMessages(Napi::Env env, SteamNetworkingMessage_t **messages, int count) {
	Napi::Array result = Napi::Array::New(env, count);
	for (int index = 0; index < count; ++index) {
		SteamNetworkingMessage_t *message = messages[index];
		Napi::Object value = JS_OBJECT;
		value.Set(
		    "data", Napi::Buffer<char>::Copy(env, static_cast<char *>(message->m_pData), message->m_cbSize)
		);
		value.Set("connection", message->m_conn);
		value.Set("steamIdRemote", jsStringFromUint64(env, message->m_identityPeer.GetSteamID64()));
		value.Set("channel", message->m_nChannel);
		value.Set("messageNumber", jsStringFromUint64(env, static_cast<uint64>(message->m_nMessageNumber)));
		value.Set("receivedAt", jsStringFromUint64(env, static_cast<uint64>(message->m_usecTimeReceived)));
		result.Set(index, value);
		message->Release();
	}
	return result;
}

JS_METHOD(createListenSocketP2P) {
	NAPI_ENV;
	USE_INT32_ARG(0, localVirtualPort, 0);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->CreateListenSocketP2P(localVirtualPort, 0, nullptr));
}

JS_METHOD(connectP2P) {
	NAPI_ENV;
	SteamNetworkingIdentity identity;
	if (!requireIdentity(env, info, 0, "steamId", &identity))
		RET_UNDEFINED;
	USE_INT32_ARG(1, remoteVirtualPort, 0);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->ConnectP2P(identity, remoteVirtualPort, 0, nullptr));
}

JS_METHOD(acceptConnection) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->AcceptConnection(connection));
}
JS_METHOD(closeConnection) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	USE_INT32_ARG(1, reason, 0);
	USE_STR_ARG(2, debug, "");
	USE_BOOL_ARG(3, linger, false);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->CloseConnection(connection, reason, debug.c_str(), linger));
}
JS_METHOD(closeListenSocket) {
	NAPI_ENV;
	REQ_INT32_ARG(0, listenSocket);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->CloseListenSocket(listenSocket));
}
JS_METHOD(setConnectionName) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	REQ_STR_ARG(1, name);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	value->SetConnectionName(connection, name.c_str());
	RET_UNDEFINED;
}
JS_METHOD(getConnectionName) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	char name[256] = {};
	if (!value->GetConnectionName(connection, name, sizeof(name)))
		RET_NULL;
	RET_STR(name);
}
JS_METHOD(sendMessageToConnection) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	if (info.Length() <= 1 || !info[1].IsBuffer()) {
		JS_THROW("data must be a Buffer.");
		RET_UNDEFINED;
	}
	auto data = info[1].As<Napi::Buffer<char>>();
	if (data.Length() > k_cbMaxSteamNetworkingSocketsMessageSizeSend) {
		JS_THROW("data exceeds the maximum Steam networking message size.");
		RET_UNDEFINED;
	}
	USE_INT32_ARG(2, sendFlags, k_nSteamNetworkingSend_Reliable);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	int64 number = 0;
	Napi::Object result = JS_OBJECT;
	result.Set(
	    "result",
	    static_cast<int32>(value->SendMessageToConnection(
	        connection, data.Data(), static_cast<uint32>(data.Length()), sendFlags, &number
	    ))
	);
	result.Set("messageNumber", jsStringFromUint64(env, static_cast<uint64>(number)));
	RET_VALUE(result);
}
JS_METHOD(flushMessagesOnConnection) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->FlushMessagesOnConnection(connection));
}
JS_METHOD(receiveMessagesOnConnection) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	USE_INT32_ARG(1, maximumMessages, 32);
	if (maximumMessages < 1 || maximumMessages > 256) {
		JS_THROW("maximumMessages must be between 1 and 256.");
		RET_UNDEFINED;
	}
	std::vector<SteamNetworkingMessage_t *> messages(maximumMessages);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	int count = value->ReceiveMessagesOnConnection(connection, messages.data(), maximumMessages);
	if (count < 0) {
		JS_THROW("connection is invalid.");
		RET_UNDEFINED;
	}
	RET_VALUE(copyAndReleaseMessages(env, messages.data(), count));
}
JS_METHOD(createPollGroup) {
	NAPI_ENV;
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->CreatePollGroup());
}
JS_METHOD(destroyPollGroup) {
	NAPI_ENV;
	REQ_INT32_ARG(0, pollGroup);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->DestroyPollGroup(pollGroup));
}
JS_METHOD(setConnectionPollGroup) {
	NAPI_ENV;
	REQ_INT32_ARG(0, connection);
	REQ_INT32_ARG(1, pollGroup);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->SetConnectionPollGroup(connection, pollGroup));
}
JS_METHOD(receiveMessagesOnPollGroup) {
	NAPI_ENV;
	REQ_INT32_ARG(0, pollGroup);
	USE_INT32_ARG(1, maximumMessages, 32);
	if (maximumMessages < 1 || maximumMessages > 256) {
		JS_THROW("maximumMessages must be between 1 and 256.");
		RET_UNDEFINED;
	}
	std::vector<SteamNetworkingMessage_t *> messages(maximumMessages);
	auto *value = steamNetworkingSockets(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	int count = value->ReceiveMessagesOnPollGroup(pollGroup, messages.data(), maximumMessages);
	if (count < 0) {
		JS_THROW("pollGroup is invalid.");
		RET_UNDEFINED;
	}
	RET_VALUE(copyAndReleaseMessages(env, messages.data(), count));
}
JS_METHOD(sendMessageToUser) {
	NAPI_ENV;
	SteamNetworkingIdentity identity;
	if (!requireIdentity(env, info, 0, "steamId", &identity))
		RET_UNDEFINED;
	if (info.Length() <= 1 || !info[1].IsBuffer()) {
		JS_THROW("data must be a Buffer.");
		RET_UNDEFINED;
	}
	auto data = info[1].As<Napi::Buffer<char>>();
	if (data.Length() > k_cbMaxSteamNetworkingSocketsMessageSizeSend) {
		JS_THROW("data exceeds the maximum Steam networking message size.");
		RET_UNDEFINED;
	}
	USE_INT32_ARG(2, sendFlags, k_nSteamNetworkingSend_Reliable);
	USE_INT32_ARG(3, channel, 0);
	auto *value = steamNetworkingMessages(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_NUM(value->SendMessageToUser(
	    identity, data.Data(), static_cast<uint32>(data.Length()), sendFlags, channel
	));
}
JS_METHOD(receiveMessagesOnChannel) {
	NAPI_ENV;
	USE_INT32_ARG(0, channel, 0);
	USE_INT32_ARG(1, maximumMessages, 32);
	if (maximumMessages < 1 || maximumMessages > 256) {
		JS_THROW("maximumMessages must be between 1 and 256.");
		RET_UNDEFINED;
	}
	std::vector<SteamNetworkingMessage_t *> messages(maximumMessages);
	auto *value = steamNetworkingMessages(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	int count = value->ReceiveMessagesOnChannel(channel, messages.data(), maximumMessages);
	RET_VALUE(copyAndReleaseMessages(env, messages.data(), count));
}
JS_METHOD(acceptSessionWithUser) {
	NAPI_ENV;
	SteamNetworkingIdentity identity;
	if (!requireIdentity(env, info, 0, "steamId", &identity))
		RET_UNDEFINED;
	auto *value = steamNetworkingMessages(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->AcceptSessionWithUser(identity));
}
JS_METHOD(closeSessionWithUser) {
	NAPI_ENV;
	SteamNetworkingIdentity identity;
	if (!requireIdentity(env, info, 0, "steamId", &identity))
		RET_UNDEFINED;
	auto *value = steamNetworkingMessages(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->CloseSessionWithUser(identity));
}
JS_METHOD(closeChannelWithUser) {
	NAPI_ENV;
	SteamNetworkingIdentity identity;
	if (!requireIdentity(env, info, 0, "steamId", &identity))
		RET_UNDEFINED;
	REQ_INT32_ARG(1, channel);
	auto *value = steamNetworkingMessages(env);
	if (env.IsExceptionPending())
		RET_UNDEFINED;
	RET_BOOL(value->CloseChannelWithUser(identity, channel));
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

Napi::Object createSocketsNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("createListenSocketP2P", Napi::Function::New(env, createListenSocketP2P));
	value.Set("connectP2P", Napi::Function::New(env, connectP2P));
	value.Set("acceptConnection", Napi::Function::New(env, acceptConnection));
	value.Set("closeConnection", Napi::Function::New(env, closeConnection));
	value.Set("closeListenSocket", Napi::Function::New(env, closeListenSocket));
	value.Set("setConnectionName", Napi::Function::New(env, setConnectionName));
	value.Set("getConnectionName", Napi::Function::New(env, getConnectionName));
	value.Set("sendMessageToConnection", Napi::Function::New(env, sendMessageToConnection));
	value.Set("flushMessagesOnConnection", Napi::Function::New(env, flushMessagesOnConnection));
	value.Set("receiveMessagesOnConnection", Napi::Function::New(env, receiveMessagesOnConnection));
	value.Set("createPollGroup", Napi::Function::New(env, createPollGroup));
	value.Set("destroyPollGroup", Napi::Function::New(env, destroyPollGroup));
	value.Set("setConnectionPollGroup", Napi::Function::New(env, setConnectionPollGroup));
	value.Set("receiveMessagesOnPollGroup", Napi::Function::New(env, receiveMessagesOnPollGroup));
	return value;
}

Napi::Object createMessagesNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("sendMessageToUser", Napi::Function::New(env, sendMessageToUser));
	value.Set("receiveMessagesOnChannel", Napi::Function::New(env, receiveMessagesOnChannel));
	value.Set("acceptSessionWithUser", Napi::Function::New(env, acceptSessionWithUser));
	value.Set("closeSessionWithUser", Napi::Function::New(env, closeSessionWithUser));
	value.Set("closeChannelWithUser", Napi::Function::New(env, closeChannelWithUser));
	return value;
}
} // namespace steam_api::networking
