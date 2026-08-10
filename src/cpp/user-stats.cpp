#include "user-stats.hpp"

#include "call-result.hpp"

#include <cmath>
#include <limits>

namespace steam_api::user_stats {
ISteamUserStats *steamUserStats(Napi::Env env) {
	ISteamUserStats *value = SteamUserStats();
	if (value == nullptr) {
		JS_THROW("SteamUserStats is not available. Call steam.initEx() first.");
	}
	return value;
}

namespace {
class NumberOfCurrentPlayersRequest
    : public PendingCallResultRequest<NumberOfCurrentPlayersRequest, NumberOfCurrentPlayers_t> {
  public:
	explicit NumberOfCurrentPlayersRequest(Napi::Env env) : PendingCallResultRequest(env) {}

	void onCompleted(NumberOfCurrentPlayers_t *param, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Error on getting current players: Steam API IO failure.").Value()
			);
			return;
		}
		if (param->m_bSuccess == 0) {
			deferred().Reject(Napi::Error::New(env(), "Error on getting current players.").Value());
			return;
		}

		deferred().Resolve(Napi::Number::New(env(), param->m_cPlayers));
	}
};

std::vector<std::unique_ptr<NumberOfCurrentPlayersRequest>> numberOfCurrentPlayersRequests;
} // namespace

JS_METHOD(getStatInt) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	int32 result = 0;
	if (!value->GetStat(name.c_str(), &result)) {
		RET_NULL;
	}
	RET_NUM(result);
}

JS_METHOD(getStatFloat) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	float result = 0;
	if (!value->GetStat(name.c_str(), &result)) {
		RET_NULL;
	}
	RET_NUM(result);
}

JS_METHOD(setStat) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_DOUBLE_ARG(1, value);

	ISteamUserStats *stats = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	if (std::trunc(value) == value && value >= std::numeric_limits<int32_t>::min() &&
	    value <= std::numeric_limits<int32_t>::max()) {
		RET_BOOL(stats->SetStat(name.c_str(), static_cast<int32>(value)));
	}

	RET_BOOL(stats->SetStat(name.c_str(), static_cast<float>(value)));
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

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	bool achieved = false;
	if (!value->GetAchievement(name.c_str(), &achieved)) {
		RET_NULL;
	}
	RET_BOOL(achieved);
}

JS_METHOD(setAchievement) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->SetAchievement(name.c_str()));
}

JS_METHOD(clearAchievement) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->ClearAchievement(name.c_str()));
}

JS_METHOD(indicateAchievementProgress) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_UINT32_ARG(1, currentProgress);
	REQ_UINT32_ARG(2, maxProgress);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->IndicateAchievementProgress(name.c_str(), currentProgress, maxProgress));
}

JS_METHOD(storeStats) {
	NAPI_ENV;
	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_BOOL(value->StoreStats());
}

JS_METHOD(resetAllStats) {
	NAPI_ENV;
	REQ_BOOL_ARG(0, achievementsToo);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(value->ResetAllStats(achievementsToo));
}

JS_METHOD(getNumAchievements) {
	NAPI_ENV;
	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}
	RET_NUM(value->GetNumAchievements());
}

JS_METHOD(getAchievementNames) {
	NAPI_ENV;
	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 count = value->GetNumAchievements();
	Napi::Array names = Napi::Array::New(env, count);
	for (uint32 i = 0; i < count; i++) {
		const char *name = value->GetAchievementName(i);
		names.Set(i, name == nullptr ? JS_NULL : JS_STR(name));
	}
	RET_VALUE(names);
}

JS_METHOD(getAchievementName) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, index);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	const char *name = value->GetAchievementName(index);
	if (name == nullptr) {
		RET_NULL;
	}
	RET_STR(name);
}

JS_METHOD(getAchievementDisplayAttribute) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);
	REQ_STR_ARG(1, key);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	const char *result = value->GetAchievementDisplayAttribute(name.c_str(), key.c_str());
	if (result == nullptr) {
		RET_NULL;
	}
	RET_STR(result);
}

JS_METHOD(getAchievementAndUnlockTime) {
	NAPI_ENV;
	REQ_STR_ARG(0, name);

	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	bool achieved = false;
	uint32 unlockTime = 0;
	if (!value->GetAchievementAndUnlockTime(name.c_str(), &achieved, &unlockTime)) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("achieved", achieved);
	result.Set("unlockTime", unlockTime);
	RET_VALUE(result);
}

JS_METHOD(getNumberOfCurrentPlayers) {
	NAPI_ENV;
	ISteamUserStats *value = steamUserStats(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->GetNumberOfCurrentPlayers();
	RET_VALUE(trackCallResult<NumberOfCurrentPlayersRequest>(env, call, numberOfCurrentPlayersRequests));
}

void rejectPendingPromises(const std::string &message) {
	rejectPendingCallResults(numberOfCurrentPlayersRequests, message);
}

void clearPendingPromises() {
	clearPendingCallResults(numberOfCurrentPlayersRequests);
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("getStatInt", Napi::Function::New(env, getStatInt));
	value.Set("getStatFloat", Napi::Function::New(env, getStatFloat));
	value.Set("setStat", Napi::Function::New(env, setStat));
	value.Set("setStatInt", Napi::Function::New(env, setStatInt));
	value.Set("setStatFloat", Napi::Function::New(env, setStatFloat));
	value.Set("getAchievement", Napi::Function::New(env, getAchievement));
	value.Set("setAchievement", Napi::Function::New(env, setAchievement));
	value.Set("clearAchievement", Napi::Function::New(env, clearAchievement));
	value.Set("indicateAchievementProgress", Napi::Function::New(env, indicateAchievementProgress));
	value.Set("storeStats", Napi::Function::New(env, storeStats));
	value.Set("resetAllStats", Napi::Function::New(env, resetAllStats));
	value.Set("getNumAchievements", Napi::Function::New(env, getNumAchievements));
	value.Set("getAchievementNames", Napi::Function::New(env, getAchievementNames));
	value.Set("getAchievementName", Napi::Function::New(env, getAchievementName));
	value.Set("getAchievementDisplayAttribute", Napi::Function::New(env, getAchievementDisplayAttribute));
	value.Set("getAchievementAndUnlockTime", Napi::Function::New(env, getAchievementAndUnlockTime));
	value.Set("getNumberOfCurrentPlayers", Napi::Function::New(env, getNumberOfCurrentPlayers));
	return value;
}
} // namespace steam_api::user_stats
