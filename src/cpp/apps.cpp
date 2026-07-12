#include "apps.hpp"

namespace steam_api::apps {
ISteamApps *steamApps(Napi::Env env) {
	ISteamApps *value = SteamApps();
	if (value == nullptr) {
		JS_THROW("SteamApps is not available. Call steam.initEx() first.");
	}
	return value;
}

JS_METHOD(isSubscribed) {
	NAPI_ENV;
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BIsSubscribed());
}

JS_METHOD(isSubscribedApp) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BIsSubscribedApp(appId));
}

JS_METHOD(isDlcInstalled) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BIsDlcInstalled(appId));
}

JS_METHOD(isAppInstalled) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->BIsAppInstalled(appId));
}

JS_METHOD(getCurrentGameLanguage) {
	NAPI_ENV;
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(value->GetCurrentGameLanguage());
}

JS_METHOD(getAvailableGameLanguages) {
	NAPI_ENV;
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(value->GetAvailableGameLanguages());
}

JS_METHOD(getDlcCount) {
	NAPI_ENV;
	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetDLCCount());
}

JS_METHOD(getDlcDataByIndex) {
	NAPI_ENV;
	REQ_INT32_ARG(0, index);

	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	AppId_t appId = 0;
	bool available = false;
	char name[256] = {};
	bool ok = value->BGetDLCDataByIndex(index, &appId, &available, name, sizeof(name));

	if (!ok) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("appId", static_cast<uint32_t>(appId));
	result.Set("available", available);
	result.Set("name", name);
	RET_VALUE(result);
}

JS_METHOD(getAppInstallDir) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);

	ISteamApps *value = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<char> folder(4096);
	uint32 size = value->GetAppInstallDir(appId, folder.data(), static_cast<uint32>(folder.size()));
	if (size == 0) {
		RET_NULL;
	}
	RET_STR(folder.data());
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("isSubscribed", Napi::Function::New(env, isSubscribed));
	value.Set("isSubscribedApp", Napi::Function::New(env, isSubscribedApp));
	value.Set("isDlcInstalled", Napi::Function::New(env, isDlcInstalled));
	value.Set("isAppInstalled", Napi::Function::New(env, isAppInstalled));
	value.Set("getCurrentGameLanguage", Napi::Function::New(env, getCurrentGameLanguage));
	value.Set("getAvailableGameLanguages", Napi::Function::New(env, getAvailableGameLanguages));
	value.Set("getDlcCount", Napi::Function::New(env, getDlcCount));
	value.Set("getDlcDataByIndex", Napi::Function::New(env, getDlcDataByIndex));
	value.Set("getAppInstallDir", Napi::Function::New(env, getAppInstallDir));
	return value;
}
} // namespace steam_api::apps
