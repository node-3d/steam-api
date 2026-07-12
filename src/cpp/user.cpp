#include "user.hpp"

namespace steam_api::user {
ISteamUser *steamUser(Napi::Env env) {
	ISteamUser *value = SteamUser();
	if (value == nullptr) {
		JS_THROW("SteamUser is not available. Call steam.initEx() first.");
	}
	return value;
}

JS_METHOD(getHSteamUser) {
	NAPI_ENV;
	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetHSteamUser());
}

JS_METHOD(isLoggedOn) {
	NAPI_ENV;
	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BLoggedOn());
}

JS_METHOD(getSteamId) {
	NAPI_ENV;
	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_VALUE(jsStringFromUint64(env, value->GetSteamID().ConvertToUint64()));
}

JS_METHOD(getAuthSessionTicket) {
	NAPI_ENV;

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<uint8_t> ticket(4096);
	uint32 size = 0;
	HAuthTicket handle =
	    value->GetAuthSessionTicket(ticket.data(), static_cast<int>(ticket.size()), &size, nullptr);
	ticket.resize(size);

	Napi::Object result = JS_OBJECT;
	result.Set("handle", static_cast<uint32_t>(handle));
	result.Set("ticket", Napi::Buffer<uint8_t>::Copy(env, ticket.data(), ticket.size()));
	RET_VALUE(result);
}

JS_METHOD(cancelAuthTicket) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, handle);

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->CancelAuthTicket(static_cast<HAuthTicket>(handle));
	RET_UNDEFINED;
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("getHSteamUser", Napi::Function::New(env, getHSteamUser));
	value.Set("isLoggedOn", Napi::Function::New(env, isLoggedOn));
	value.Set("getSteamId", Napi::Function::New(env, getSteamId));
	value.Set("getAuthSessionTicket", Napi::Function::New(env, getAuthSessionTicket));
	value.Set("cancelAuthTicket", Napi::Function::New(env, cancelAuthTicket));
	return value;
}
} // namespace steam_api::user
