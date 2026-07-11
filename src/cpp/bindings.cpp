#include <addon-tools.hpp>
#include <steam/steam_api.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace steam_api {

namespace {

Napi::String jsStringFromUint64(Napi::Env env, uint64 value) {
	return Napi::String::New(env, std::to_string(value));
}

Napi::Object makeEvent(Napi::Env env, const std::string &type) {
	Napi::Object event = Napi::Object::New(env);
	event.Set("type", type);
	return event;
}

struct Event {
	std::string type;
	uint64 gameId = 0;
	uint64 userId = 0;
	std::string name;
	int32_t result = 0;
	uint32_t currentProgress = 0;
	uint32_t maxProgress = 0;
};

class CallbackBridge {
public:
	CallbackBridge() = default;

	void registerAll() {
		if (_isRegistered) {
			return;
		}

		_userStatsReceived.Register(this, &CallbackBridge::onUserStatsReceived);
		_userStatsStored.Register(this, &CallbackBridge::onUserStatsStored);
		_userAchievementStored.Register(this, &CallbackBridge::onUserAchievementStored);
		_authTicketResponse.Register(this, &CallbackBridge::onAuthTicketResponse);
		_isRegistered = true;
	}

	void unregisterAll() {
		if (!_isRegistered) {
			return;
		}

		_userStatsReceived.Unregister();
		_userStatsStored.Unregister();
		_userAchievementStored.Unregister();
		_authTicketResponse.Unregister();
		_isRegistered = false;
		_events.clear();
	}

	Napi::Array poll(Napi::Env env) {
		Napi::Array events = Napi::Array::New(env, _events.size());

		for (size_t i = 0; i < _events.size(); i++) {
			const Event &source = _events[i];
			Napi::Object event = makeEvent(env, source.type);

			if (source.gameId != 0) {
				event.Set("gameId", jsStringFromUint64(env, source.gameId));
			}
			if (source.userId != 0) {
				event.Set("userId", jsStringFromUint64(env, source.userId));
			}
			if (!source.name.empty()) {
				event.Set("name", source.name);
			}
			event.Set("result", source.result);
			event.Set("currentProgress", source.currentProgress);
			event.Set("maxProgress", source.maxProgress);

			events.Set(i, event);
		}

		_events.clear();
		return events;
	}

private:
	void onUserStatsReceived(UserStatsReceived_t *param) {
		Event event;
		event.type = "userStatsReceived";
		event.gameId = param->m_nGameID;
		event.userId = param->m_steamIDUser.ConvertToUint64();
		event.result = static_cast<int32_t>(param->m_eResult);
		_events.push_back(event);
	}

	void onUserStatsStored(UserStatsStored_t *param) {
		Event event;
		event.type = "userStatsStored";
		event.gameId = param->m_nGameID;
		event.result = static_cast<int32_t>(param->m_eResult);
		_events.push_back(event);
	}

	void onUserAchievementStored(UserAchievementStored_t *param) {
		Event event;
		event.type = "userAchievementStored";
		event.gameId = param->m_nGameID;
		event.name = param->m_rgchAchievementName;
		event.currentProgress = param->m_nCurProgress;
		event.maxProgress = param->m_nMaxProgress;
		_events.push_back(event);
	}

	void onAuthTicketResponse(GetAuthSessionTicketResponse_t *param) {
		Event event;
		event.type = "authSessionTicketResponse";
		event.result = static_cast<int32_t>(param->m_eResult);
		event.currentProgress = static_cast<uint32_t>(param->m_hAuthTicket);
		_events.push_back(event);
	}

	bool _isRegistered = false;
	std::vector<Event> _events;
	CCallbackManual<CallbackBridge, UserStatsReceived_t> _userStatsReceived;
	CCallbackManual<CallbackBridge, UserStatsStored_t> _userStatsStored;
	CCallbackManual<CallbackBridge, UserAchievementStored_t> _userAchievementStored;
	CCallbackManual<CallbackBridge, GetAuthSessionTicketResponse_t> _authTicketResponse;
};

std::unique_ptr<CallbackBridge> callbackBridge;
bool isInitialized = false;

void ensureCallbackBridge() {
	if (!callbackBridge) {
		callbackBridge = std::make_unique<CallbackBridge>();
	}
	callbackBridge->registerAll();
}

ISteamUser *steamUser(Napi::Env env) {
	ISteamUser *user = SteamUser();
	if (user == nullptr) {
		JS_THROW("SteamUser is not available. Call initEx() first.");
	}
	return user;
}

ISteamUtils *steamUtils(Napi::Env env) {
	ISteamUtils *utils = SteamUtils();
	if (utils == nullptr) {
		JS_THROW("SteamUtils is not available. Call initEx() first.");
	}
	return utils;
}

ISteamApps *steamApps(Napi::Env env) {
	ISteamApps *apps = SteamApps();
	if (apps == nullptr) {
		JS_THROW("SteamApps is not available. Call initEx() first.");
	}
	return apps;
}

ISteamUserStats *steamUserStats(Napi::Env env) {
	ISteamUserStats *stats = SteamUserStats();
	if (stats == nullptr) {
		JS_THROW("SteamUserStats is not available. Call initEx() first.");
	}
	return stats;
}

} // namespace

JS_METHOD(restartAppIfNecessary) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	RET_BOOL(SteamAPI_RestartAppIfNecessary(appId));
}

JS_METHOD(initEx) {
	NAPI_ENV;

	SteamErrMsg errorMessage = {};
	ESteamAPIInitResult result = SteamAPI_InitEx(&errorMessage);
	isInitialized = result == k_ESteamAPIInitResult_OK;

	if (isInitialized) {
		ensureCallbackBridge();
	}

	Napi::Object value = Napi::Object::New(env);
	value.Set("result", static_cast<int32_t>(result));
	value.Set("ok", isInitialized);
	value.Set("errorMessage", std::string(errorMessage));
	RET_VALUE(value);
}

JS_METHOD(shutdown) {
	NAPI_ENV;
	if (callbackBridge) {
		callbackBridge->unregisterAll();
	}
	SteamAPI_Shutdown();
	isInitialized = false;
	RET_UNDEFINED;
}

JS_METHOD(isSteamRunning) {
	NAPI_ENV;
	RET_BOOL(SteamAPI_IsSteamRunning());
}

JS_METHOD(runCallbacks) {
	NAPI_ENV;
	if (isInitialized) {
		ensureCallbackBridge();
		SteamAPI_RunCallbacks();
	}
	RET_UNDEFINED;
}

JS_METHOD(releaseCurrentThreadMemory) {
	NAPI_ENV;
	SteamAPI_ReleaseCurrentThreadMemory();
	RET_UNDEFINED;
}

JS_METHOD(pollCallbacks) {
	NAPI_ENV;
	if (!callbackBridge) {
		RET_VALUE(Napi::Array::New(env));
	}
	RET_VALUE(callbackBridge->poll(env));
}

JS_METHOD(getHSteamUser) {
	NAPI_ENV;
	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(user->GetHSteamUser());
}

JS_METHOD(isLoggedOn) {
	NAPI_ENV;
	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(user->BLoggedOn());
}

JS_METHOD(getSteamId) {
	NAPI_ENV;
	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_VALUE(jsStringFromUint64(env, user->GetSteamID().ConvertToUint64()));
}

JS_METHOD(getAuthSessionTicket) {
	NAPI_ENV;

	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<uint8_t> ticket(4096);
	uint32 size = 0;
	HAuthTicket handle = user->GetAuthSessionTicket(ticket.data(), static_cast<int>(ticket.size()), &size, nullptr);
	ticket.resize(size);

	Napi::Object result = Napi::Object::New(env);
	result.Set("handle", static_cast<uint32_t>(handle));
	result.Set("ticket", Napi::Buffer<uint8_t>::Copy(env, ticket.data(), ticket.size()));
	RET_VALUE(result);
}

JS_METHOD(cancelAuthTicket) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, handle);

	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	user->CancelAuthTicket(static_cast<HAuthTicket>(handle));
	RET_UNDEFINED;
}

JS_METHOD(getAppId) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(utils->GetAppID());
}

JS_METHOD(getConnectedUniverse) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(utils->GetConnectedUniverse());
}

JS_METHOD(getIpCountry) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(utils->GetIPCountry());
}

JS_METHOD(getServerRealTime) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(utils->GetServerRealTime());
}

JS_METHOD(getSteamUiLanguage) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(utils->GetSteamUILanguage());
}

JS_METHOD(isOverlayEnabled) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(utils->IsOverlayEnabled());
}

JS_METHOD(isSteamInBigPictureMode) {
	NAPI_ENV;
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(utils->IsSteamInBigPictureMode());
}

JS_METHOD(isSubscribed) {
	NAPI_ENV;
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(apps->BIsSubscribed());
}

JS_METHOD(isSubscribedApp) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(apps->BIsSubscribedApp(appId));
}

JS_METHOD(isDlcInstalled) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(apps->BIsDlcInstalled(appId));
}

JS_METHOD(isAppInstalled) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(apps->BIsAppInstalled(appId));
}

JS_METHOD(getCurrentGameLanguage) {
	NAPI_ENV;
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(apps->GetCurrentGameLanguage());
}

JS_METHOD(getAvailableGameLanguages) {
	NAPI_ENV;
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_STR(apps->GetAvailableGameLanguages());
}

JS_METHOD(getDlcCount) {
	NAPI_ENV;
	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(apps->GetDLCCount());
}

JS_METHOD(getDlcDataByIndex) {
	NAPI_ENV;
	REQ_INT32_ARG(0, index);

	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	AppId_t appId = 0;
	bool available = false;
	char name[256] = {};
	bool ok = apps->BGetDLCDataByIndex(index, &appId, &available, name, sizeof(name));

	if (!ok) {
		RET_NULL;
	}

	Napi::Object value = Napi::Object::New(env);
	value.Set("appId", static_cast<uint32_t>(appId));
	value.Set("available", available);
	value.Set("name", name);
	RET_VALUE(value);
}

JS_METHOD(getAppInstallDir) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);

	ISteamApps *apps = steamApps(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	std::vector<char> folder(4096);
	uint32 size = apps->GetAppInstallDir(appId, folder.data(), static_cast<uint32>(folder.size()));
	if (size == 0) {
		RET_NULL;
	}
	RET_STR(folder.data());
}

JS_METHOD(getStatInt) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	int32 value = 0;
	if (!stats->GetStat(name.c_str(), &value)) {
		RET_NULL;
	}
	RET_NUM(value);
}

JS_METHOD(getStatFloat) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	float value = 0;
	if (!stats->GetStat(name.c_str(), &value)) {
		RET_NULL;
	}
	RET_NUM(value);
}

JS_METHOD(setStatInt) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_INT32_ARG(1, value);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(stats->SetStat(name.c_str(), value));
}

JS_METHOD(setStatFloat) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_FLOAT_ARG(1, value);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(stats->SetStat(name.c_str(), value));
}

JS_METHOD(getAchievement) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	bool achieved = false;
	if (!stats->GetAchievement(name.c_str(), &achieved)) {
		RET_NULL;
	}
	RET_BOOL(achieved);
}

JS_METHOD(setAchievement) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(stats->SetAchievement(name.c_str()));
}

JS_METHOD(clearAchievement) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(stats->ClearAchievement(name.c_str()));
}

JS_METHOD(storeStats) {
	NAPI_ENV;
	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(stats->StoreStats());
}

JS_METHOD(resetAllStats) {
	NAPI_ENV;
	REQ_BOOL_ARG(0, achievementsToo);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(stats->ResetAllStats(achievementsToo));
}

JS_METHOD(getNumAchievements) {
	NAPI_ENV;
	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(stats->GetNumAchievements());
}

JS_METHOD(getAchievementName) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, index);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	const char *name = stats->GetAchievementName(index);
	if (name == nullptr) {
		RET_NULL;
	}
	RET_STR(name);
}

JS_METHOD(getAchievementDisplayAttribute) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_STR_ARG(1, key);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	const char *value = stats->GetAchievementDisplayAttribute(name.c_str(), key.c_str());
	if (value == nullptr) {
		RET_NULL;
	}
	RET_STR(value);
}

JS_METHOD(getAchievementAndUnlockTime) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	bool achieved = false;
	uint32 unlockTime = 0;
	if (!stats->GetAchievementAndUnlockTime(name.c_str(), &achieved, &unlockTime)) {
		RET_NULL;
	}

	Napi::Object value = Napi::Object::New(env);
	value.Set("achieved", achieved);
	value.Set("unlockTime", unlockTime);
	RET_VALUE(value);
}

#define JS_STEAM_SET_METHOD(name)                                             \
	exports.DefineProperty(                                                   \
		Napi::PropertyDescriptor::Function(env, exports, #name, steam_api::name, napi_writable) \
	);

#define JS_STEAM_CONSTANT(name) exports.Set(#name, static_cast<int32_t>(name));

} // namespace steam_api

Napi::Object initModule(Napi::Env env, Napi::Object exports) {
	JS_STEAM_SET_METHOD(restartAppIfNecessary);
	JS_STEAM_SET_METHOD(initEx);
	JS_STEAM_SET_METHOD(shutdown);
	JS_STEAM_SET_METHOD(isSteamRunning);
	JS_STEAM_SET_METHOD(runCallbacks);
	JS_STEAM_SET_METHOD(releaseCurrentThreadMemory);
	JS_STEAM_SET_METHOD(pollCallbacks);
	JS_STEAM_SET_METHOD(getHSteamUser);
	JS_STEAM_SET_METHOD(isLoggedOn);
	JS_STEAM_SET_METHOD(getSteamId);
	JS_STEAM_SET_METHOD(getAuthSessionTicket);
	JS_STEAM_SET_METHOD(cancelAuthTicket);
	JS_STEAM_SET_METHOD(getAppId);
	JS_STEAM_SET_METHOD(getConnectedUniverse);
	JS_STEAM_SET_METHOD(getIpCountry);
	JS_STEAM_SET_METHOD(getServerRealTime);
	JS_STEAM_SET_METHOD(getSteamUiLanguage);
	JS_STEAM_SET_METHOD(isOverlayEnabled);
	JS_STEAM_SET_METHOD(isSteamInBigPictureMode);
	JS_STEAM_SET_METHOD(isSubscribed);
	JS_STEAM_SET_METHOD(isSubscribedApp);
	JS_STEAM_SET_METHOD(isDlcInstalled);
	JS_STEAM_SET_METHOD(isAppInstalled);
	JS_STEAM_SET_METHOD(getCurrentGameLanguage);
	JS_STEAM_SET_METHOD(getAvailableGameLanguages);
	JS_STEAM_SET_METHOD(getDlcCount);
	JS_STEAM_SET_METHOD(getDlcDataByIndex);
	JS_STEAM_SET_METHOD(getAppInstallDir);
	JS_STEAM_SET_METHOD(getStatInt);
	JS_STEAM_SET_METHOD(getStatFloat);
	JS_STEAM_SET_METHOD(setStatInt);
	JS_STEAM_SET_METHOD(setStatFloat);
	JS_STEAM_SET_METHOD(getAchievement);
	JS_STEAM_SET_METHOD(setAchievement);
	JS_STEAM_SET_METHOD(clearAchievement);
	JS_STEAM_SET_METHOD(storeStats);
	JS_STEAM_SET_METHOD(resetAllStats);
	JS_STEAM_SET_METHOD(getNumAchievements);
	JS_STEAM_SET_METHOD(getAchievementName);
	JS_STEAM_SET_METHOD(getAchievementDisplayAttribute);
	JS_STEAM_SET_METHOD(getAchievementAndUnlockTime);

	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_OK);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_FailedGeneric);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_NoSteamClient);
	JS_STEAM_CONSTANT(k_ESteamAPIInitResult_VersionMismatch);
	JS_STEAM_CONSTANT(k_EResultOK);
	JS_STEAM_CONSTANT(k_EResultFail);
	JS_STEAM_CONSTANT(k_EUniverseInvalid);
	JS_STEAM_CONSTANT(k_EUniversePublic);
	JS_STEAM_CONSTANT(k_EUniverseBeta);
	JS_STEAM_CONSTANT(k_EUniverseInternal);
	JS_STEAM_CONSTANT(k_EUniverseDev);

	return exports;
}

NODE_API_MODULE(steam_api, initModule)
