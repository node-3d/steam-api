#pragma once

#include "common.hpp"

namespace steam_api::apps {
JS_METHOD(isSubscribed);
JS_METHOD(isSubscribedApp);
JS_METHOD(isDlcInstalled);
JS_METHOD(isAppInstalled);
JS_METHOD(installDlc);
JS_METHOD(uninstallDlc);
JS_METHOD(getAppBuildId);
JS_METHOD(getCurrentGameLanguage);
JS_METHOD(getAvailableGameLanguages);
JS_METHOD(getCurrentGameInstallDir);
JS_METHOD(getDlcCount);
JS_METHOD(getDlcDataByIndex);
JS_METHOD(getAppInstallDir);
JS_METHOD(getLaunchCommandLine);

Napi::Object createNamespace(Napi::Env env);
} // namespace steam_api::apps
