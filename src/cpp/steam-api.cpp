#include "steam-api.hpp"

#include "callbacks.hpp"

namespace steam_api {
bool initialized = false;

bool isInitialized() {
	return initialized;
}

void setInitialized(bool value) {
	initialized = value;
}
} // namespace steam_api

namespace steam_api::steam {
JS_METHOD(restartAppIfNecessary) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	RET_BOOL(SteamAPI_RestartAppIfNecessary(appId));
}

JS_METHOD(initEx) {
	NAPI_ENV;

	SteamErrMsg errorMessage = {};
	ESteamAPIInitResult result = SteamAPI_InitEx(&errorMessage);
	setInitialized(result == k_ESteamAPIInitResult_OK);

	if (isInitialized()) {
		callbacks::ensureBridge();
	}

	Napi::Object value = JS_OBJECT;
	value.Set("result", static_cast<int32_t>(result));
	value.Set("ok", isInitialized());
	value.Set("errorMessage", std::string(errorMessage));
	RET_VALUE(value);
}

JS_METHOD(shutdown) {
	NAPI_ENV;
	callbacks::unregisterBridge();
	SteamAPI_Shutdown();
	setInitialized(false);
	RET_UNDEFINED;
}

JS_METHOD(isSteamRunning) {
	NAPI_ENV;
	RET_BOOL(SteamAPI_IsSteamRunning());
}

JS_METHOD(runCallbacks) {
	NAPI_ENV;
	if (isInitialized()) {
		callbacks::ensureBridge();
		SteamAPI_RunCallbacks();
	}
	RET_UNDEFINED;
}

JS_METHOD(releaseCurrentThreadMemory) {
	NAPI_ENV;
	SteamAPI_ReleaseCurrentThreadMemory();
	RET_UNDEFINED;
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("restartAppIfNecessary", Napi::Function::New(env, restartAppIfNecessary));
	value.Set("initEx", Napi::Function::New(env, initEx));
	value.Set("shutdown", Napi::Function::New(env, shutdown));
	value.Set("isSteamRunning", Napi::Function::New(env, isSteamRunning));
	value.Set("runCallbacks", Napi::Function::New(env, runCallbacks));
	value.Set("releaseCurrentThreadMemory", Napi::Function::New(env, releaseCurrentThreadMemory));
	return value;
}
} // namespace steam_api::steam
