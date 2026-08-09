#include "callbacks.hpp"

#include <utility>

namespace steam_api::callbacks {
enum class EventFieldKind {
	Bool,
	Int32,
	Uint32,
	Uint64String,
	String,
};

struct EventField {
	std::string name;
	EventFieldKind kind;
	bool boolValue = false;
	int32_t int32Value = 0;
	uint32_t uint32Value = 0;
	uint64 uint64Value = 0;
	std::string stringValue;
};

struct Event {
	std::string type;
	std::vector<EventField> fields;
};

EventField makeBoolField(const std::string &name, bool value) {
	EventField field;
	field.name = name;
	field.kind = EventFieldKind::Bool;
	field.boolValue = value;
	return field;
}

EventField makeInt32Field(const std::string &name, int32_t value) {
	EventField field;
	field.name = name;
	field.kind = EventFieldKind::Int32;
	field.int32Value = value;
	return field;
}

EventField makeUint32Field(const std::string &name, uint32_t value) {
	EventField field;
	field.name = name;
	field.kind = EventFieldKind::Uint32;
	field.uint32Value = value;
	return field;
}

EventField makeUint64StringField(const std::string &name, uint64 value) {
	EventField field;
	field.name = name;
	field.kind = EventFieldKind::Uint64String;
	field.uint64Value = value;
	return field;
}

EventField makeStringField(const std::string &name, const std::string &value) {
	EventField field;
	field.name = name;
	field.kind = EventFieldKind::String;
	field.stringValue = value;
	return field;
}

Event makeEventData(std::string type) {
	Event event;
	event.type = std::move(type);
	return event;
}

void setEventField(Napi::Env env, Napi::Object event, const EventField &field) {
	switch (field.kind) {
		case EventFieldKind::Bool:
			event.Set(field.name, Napi::Boolean::New(env, field.boolValue));
			break;
		case EventFieldKind::Int32:
			event.Set(field.name, Napi::Number::New(env, field.int32Value));
			break;
		case EventFieldKind::Uint32:
			event.Set(field.name, Napi::Number::New(env, field.uint32Value));
			break;
		case EventFieldKind::Uint64String:
			event.Set(field.name, jsStringFromUint64(env, field.uint64Value));
			break;
		case EventFieldKind::String:
			event.Set(field.name, Napi::String::New(env, field.stringValue));
			break;
	}
}

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
		_gameOverlayActivated.Register(this, &CallbackBridge::onGameOverlayActivated);
		_steamServersConnected.Register(this, &CallbackBridge::onSteamServersConnected);
		_steamServersDisconnected.Register(this, &CallbackBridge::onSteamServersDisconnected);
		_steamServerConnectFailure.Register(this, &CallbackBridge::onSteamServerConnectFailure);
		_steamShutdown.Register(this, &CallbackBridge::onSteamShutdown);
		_dlcInstalled.Register(this, &CallbackBridge::onDlcInstalled);
		_newUrlLaunchParameters.Register(this, &CallbackBridge::onNewUrlLaunchParameters);
		_floatingGamepadTextInputDismissed.Register(
		    this, &CallbackBridge::onFloatingGamepadTextInputDismissed
		);
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
		_gameOverlayActivated.Unregister();
		_steamServersConnected.Unregister();
		_steamServersDisconnected.Unregister();
		_steamServerConnectFailure.Unregister();
		_steamShutdown.Unregister();
		_dlcInstalled.Unregister();
		_newUrlLaunchParameters.Unregister();
		_floatingGamepadTextInputDismissed.Unregister();
		_isRegistered = false;
		_events.clear();
	}

	Napi::Array poll(Napi::Env env) {
		Napi::Array events = Napi::Array::New(env, _events.size());

		for (size_t i = 0; i < _events.size(); i++) {
			const Event &source = _events[i];
			Napi::Object event = makeEvent(env, source.type);

			for (const EventField &field : source.fields) {
				setEventField(env, event, field);
			}

			events.Set(i, event);
		}

		_events.clear();
		return events;
	}

  private:
	void onUserStatsReceived(UserStatsReceived_t *param) {
		Event event = makeEventData("user-stats-received");
		event.fields.push_back(makeUint64StringField("gameId", param->m_nGameID));
		event.fields.push_back(makeUint64StringField("userId", param->m_steamIDUser.ConvertToUint64()));
		event.fields.push_back(makeInt32Field("result", static_cast<int32_t>(param->m_eResult)));
		_events.push_back(event);
	}

	void onUserStatsStored(UserStatsStored_t *param) {
		Event event = makeEventData("user-stats-stored");
		event.fields.push_back(makeUint64StringField("gameId", param->m_nGameID));
		event.fields.push_back(makeInt32Field("result", static_cast<int32_t>(param->m_eResult)));
		_events.push_back(event);
	}

	void onUserAchievementStored(UserAchievementStored_t *param) {
		Event event = makeEventData("user-achievement-stored");
		event.fields.push_back(makeUint64StringField("gameId", param->m_nGameID));
		event.fields.push_back(makeStringField("name", param->m_rgchAchievementName));
		event.fields.push_back(makeUint32Field("currentProgress", param->m_nCurProgress));
		event.fields.push_back(makeUint32Field("maxProgress", param->m_nMaxProgress));
		_events.push_back(event);
	}

	void onAuthTicketResponse(GetAuthSessionTicketResponse_t *param) {
		Event event = makeEventData("auth-session-ticket-response");
		event.fields.push_back(makeUint32Field("handle", static_cast<uint32_t>(param->m_hAuthTicket)));
		event.fields.push_back(makeInt32Field("result", static_cast<int32_t>(param->m_eResult)));
		_events.push_back(event);
	}

	void onGameOverlayActivated(GameOverlayActivated_t *param) {
		Event event = makeEventData("game-overlay-activated");
		event.fields.push_back(makeBoolField("active", param->m_bActive != 0));
		event.fields.push_back(makeBoolField("userInitiated", param->m_bUserInitiated));
		event.fields.push_back(makeUint32Field("appId", param->m_nAppID));
		event.fields.push_back(makeUint32Field("overlayPid", param->m_dwOverlayPID));
		_events.push_back(event);
	}

	void onSteamServersConnected(SteamServersConnected_t *param) {
		(void)param;
		_events.push_back(makeEventData("steam-servers-connected"));
	}

	void onSteamServersDisconnected(SteamServersDisconnected_t *param) {
		Event event = makeEventData("steam-servers-disconnected");
		event.fields.push_back(makeInt32Field("result", static_cast<int32_t>(param->m_eResult)));
		_events.push_back(event);
	}

	void onSteamServerConnectFailure(SteamServerConnectFailure_t *param) {
		Event event = makeEventData("steam-server-connect-failure");
		event.fields.push_back(makeInt32Field("result", static_cast<int32_t>(param->m_eResult)));
		event.fields.push_back(makeBoolField("stillRetrying", param->m_bStillRetrying));
		_events.push_back(event);
	}

	void onSteamShutdown(SteamShutdown_t *param) {
		(void)param;
		_events.push_back(makeEventData("steam-shutdown"));
	}

	void onDlcInstalled(DlcInstalled_t *param) {
		Event event = makeEventData("dlc-installed");
		event.fields.push_back(makeUint32Field("appId", param->m_nAppID));
		_events.push_back(event);
	}

	void onNewUrlLaunchParameters(NewUrlLaunchParameters_t *param) {
		(void)param;
		_events.push_back(makeEventData("new-url-launch-parameters"));
	}

	void onFloatingGamepadTextInputDismissed(FloatingGamepadTextInputDismissed_t *param) {
		(void)param;
		_events.push_back(makeEventData("floating-gamepad-text-input-dismissed"));
	}

	bool _isRegistered = false;
	std::vector<Event> _events;
	CCallbackManual<CallbackBridge, UserStatsReceived_t> _userStatsReceived;
	CCallbackManual<CallbackBridge, UserStatsStored_t> _userStatsStored;
	CCallbackManual<CallbackBridge, UserAchievementStored_t> _userAchievementStored;
	CCallbackManual<CallbackBridge, GetAuthSessionTicketResponse_t> _authTicketResponse;
	CCallbackManual<CallbackBridge, GameOverlayActivated_t> _gameOverlayActivated;
	CCallbackManual<CallbackBridge, SteamServersConnected_t> _steamServersConnected;
	CCallbackManual<CallbackBridge, SteamServersDisconnected_t> _steamServersDisconnected;
	CCallbackManual<CallbackBridge, SteamServerConnectFailure_t> _steamServerConnectFailure;
	CCallbackManual<CallbackBridge, SteamShutdown_t> _steamShutdown;
	CCallbackManual<CallbackBridge, DlcInstalled_t> _dlcInstalled;
	CCallbackManual<CallbackBridge, NewUrlLaunchParameters_t> _newUrlLaunchParameters;
	CCallbackManual<CallbackBridge, FloatingGamepadTextInputDismissed_t> _floatingGamepadTextInputDismissed;
};

std::unique_ptr<CallbackBridge> callbackBridge;

void ensureBridge() {
	if (!callbackBridge) {
		callbackBridge = std::make_unique<CallbackBridge>();
	}
	callbackBridge->registerAll();
}

void unregisterBridge() {
	if (callbackBridge) {
		callbackBridge->unregisterAll();
	}
}

JS_METHOD(pollCallbacks) {
	NAPI_ENV;
	if (!callbackBridge) {
		RET_VALUE(JS_ARRAY);
	}
	RET_VALUE(callbackBridge->poll(env));
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("pollCallbacks", Napi::Function::New(env, pollCallbacks));
	return value;
}
} // namespace steam_api::callbacks
