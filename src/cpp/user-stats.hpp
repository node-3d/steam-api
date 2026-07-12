#pragma once

#include "common.hpp"

namespace steam_api::user_stats {
JS_METHOD(getStatInt);
JS_METHOD(getStatFloat);
JS_METHOD(setStatInt);
JS_METHOD(setStatFloat);
JS_METHOD(getAchievement);
JS_METHOD(setAchievement);
JS_METHOD(clearAchievement);
JS_METHOD(storeStats);
JS_METHOD(resetAllStats);
JS_METHOD(getNumAchievements);
JS_METHOD(getAchievementName);
JS_METHOD(getAchievementDisplayAttribute);
JS_METHOD(getAchievementAndUnlockTime);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::user_stats
