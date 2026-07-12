#include "utils.hpp"

namespace steam_api::utils {
ISteamUtils *steamUtils(Napi::Env env) {
	ISteamUtils *value = SteamUtils();
	if (value == nullptr) {
		JS_THROW("SteamUtils is not available. Call steam.initEx() first.");
	}
	return value;
}

JS_METHOD(getAppId) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetAppID());
}

JS_METHOD(getConnectedUniverse) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetConnectedUniverse());
}

JS_METHOD(getIpCountry) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(value->GetIPCountry());
}

JS_METHOD(getServerRealTime) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetServerRealTime());
}

JS_METHOD(getSteamUiLanguage) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(value->GetSteamUILanguage());
}

JS_METHOD(isOverlayEnabled) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->IsOverlayEnabled());
}

JS_METHOD(isSteamInBigPictureMode) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->IsSteamInBigPictureMode());
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("getAppId", Napi::Function::New(env, getAppId));
	value.Set("getConnectedUniverse", Napi::Function::New(env, getConnectedUniverse));
	value.Set("getIpCountry", Napi::Function::New(env, getIpCountry));
	value.Set("getServerRealTime", Napi::Function::New(env, getServerRealTime));
	value.Set("getSteamUiLanguage", Napi::Function::New(env, getSteamUiLanguage));
	value.Set("isOverlayEnabled", Napi::Function::New(env, isOverlayEnabled));
	value.Set("isSteamInBigPictureMode", Napi::Function::New(env, isSteamInBigPictureMode));
	return value;
}
} // namespace steam_api::utils
