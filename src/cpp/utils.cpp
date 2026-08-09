#include "utils.hpp"

#include <limits>

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

JS_METHOD(getImageSize) {
	NAPI_ENV;
	REQ_INT32_ARG(0, image);

	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 width = 0;
	uint32 height = 0;
	if (!value->GetImageSize(image, &width, &height)) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("width", width);
	result.Set("height", height);
	RET_VALUE(result);
}

JS_METHOD(getImageRgba) {
	NAPI_ENV;
	REQ_INT32_ARG(0, image);

	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 width = 0;
	uint32 height = 0;
	if (!value->GetImageSize(image, &width, &height)) {
		RET_NULL;
	}

	const size_t byteLength = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
	if (byteLength > static_cast<size_t>(std::numeric_limits<int>::max())) {
		JS_THROW("Steam image is too large to copy into a Buffer.");
		RET_UNDEFINED;
	}

	std::vector<uint8> data(byteLength);
	if (!value->GetImageRGBA(image, data.data(), static_cast<int>(data.size()))) {
		RET_NULL;
	}

	RET_VALUE(Napi::Buffer<uint8>::Copy(env, data.data(), data.size()));
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

JS_METHOD(isSteamRunningOnSteamDeck) {
	NAPI_ENV;
	ISteamUtils *value = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->IsSteamRunningOnSteamDeck());
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("getAppId", Napi::Function::New(env, getAppId));
	value.Set("getConnectedUniverse", Napi::Function::New(env, getConnectedUniverse));
	value.Set("getIpCountry", Napi::Function::New(env, getIpCountry));
	value.Set("getServerRealTime", Napi::Function::New(env, getServerRealTime));
	value.Set("getSteamUiLanguage", Napi::Function::New(env, getSteamUiLanguage));
	value.Set("getImageSize", Napi::Function::New(env, getImageSize));
	value.Set("getImageRgba", Napi::Function::New(env, getImageRgba));
	value.Set("isOverlayEnabled", Napi::Function::New(env, isOverlayEnabled));
	value.Set("isSteamInBigPictureMode", Napi::Function::New(env, isSteamInBigPictureMode));
	value.Set("isSteamRunningOnSteamDeck", Napi::Function::New(env, isSteamRunningOnSteamDeck));
	return value;
}
} // namespace steam_api::utils
