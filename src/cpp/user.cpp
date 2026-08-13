#include "user.hpp"

#include "call-result.hpp"

#include <algorithm>
#include <steam/steamencryptedappticket.h>

namespace steam_api::user {
ISteamUser *steamUser(Napi::Env env) {
	ISteamUser *value = SteamUser();
	if (value == nullptr) {
		JS_THROW("SteamUser is not available. Call steam.initEx() first.");
	}
	return value;
}

namespace {
constexpr size_t kAuthSessionTicketBufferSize = 4096;

bool requireSteamId(
    Napi::Env env, const Napi::CallbackInfo &info, size_t index, const char *name, CSteamID *target
) {
	if (info.Length() <= index || !info[index].IsString()) {
		JS_THROW(std::string(name) + " must be a SteamID decimal string.");
		return false;
	}

	uint64 value = 0;
	if (!uint64FromJsString(env, info[index].As<Napi::String>(), name, &value)) {
		return false;
	}

	*target = CSteamID(value);
	return true;
}

bool requireBuffer(Napi::Env env, const Napi::CallbackInfo &info, size_t index, const char *name) {
	if (info.Length() <= index || !info[index].IsBuffer()) {
		JS_THROW(std::string(name) + " must be a Buffer.");
		return false;
	}

	return true;
}

Napi::Object makeAuthSessionTicket(Napi::Env env, HAuthTicket handle, const uint8_t *data, size_t size) {
	Napi::Object result = JS_OBJECT;
	result.Set("handle", static_cast<uint32_t>(handle));
	result.Set("ticket", Napi::Buffer<uint8_t>::Copy(env, data, size));
	return result;
}

Napi::Promise rejectedPromise(Napi::Env env, const std::string &message) {
	Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
	deferred.Reject(Napi::Error::New(env, message).Value());
	return deferred.Promise();
}

class WebApiAuthTicketRequest {
  public:
	WebApiAuthTicketRequest(Napi::Env env, HAuthTicket handle)
	    : _env(env), _deferred(Napi::Promise::Deferred::New(env)), _handle(handle) {}

	Napi::Promise promise() {
		return _deferred.Promise();
	}

	HAuthTicket handle() const {
		return _handle;
	}

	bool isCompleted() const {
		return _isCompleted;
	}

	void resolve(GetTicketForWebApiResponse_t *param) {
		if (_isCompleted) {
			return;
		}

		_isCompleted = true;
		Napi::HandleScope scope(_env);
		if (param->m_eResult != k_EResultOK) {
			_deferred.Reject(Napi::Error::New(_env, "Error on getting auth ticket for Web API.").Value());
			return;
		}

		_deferred.Resolve(makeAuthSessionTicket(
		    _env,
		    param->m_hAuthTicket,
		    param->m_rgubTicket,
		    std::min(
		        static_cast<size_t>(param->m_cubTicket),
		        static_cast<size_t>(GetTicketForWebApiResponse_t::k_nCubTicketMaxLength)
		    )
		));
	}

	void rejectPending(const std::string &message) {
		if (_isCompleted) {
			return;
		}

		_isCompleted = true;
		Napi::HandleScope scope(_env);
		_deferred.Reject(Napi::Error::New(_env, message).Value());
	}

  private:
	Napi::Env _env;
	Napi::Promise::Deferred _deferred;
	HAuthTicket _handle = k_HAuthTicketInvalid;
	bool _isCompleted = false;
};

class EncryptedAppTicketRequest
    : public PendingCallResultRequest<EncryptedAppTicketRequest, EncryptedAppTicketResponse_t> {
  public:
	explicit EncryptedAppTicketRequest(Napi::Env env) : PendingCallResultRequest(env) {}

	void onCompleted(EncryptedAppTicketResponse_t *param, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure || param->m_eResult != k_EResultOK) {
			deferred().Reject(Napi::Error::New(env(), "Error on getting encrypted app ticket.").Value());
			return;
		}

		ISteamUser *value = SteamUser();
		if (value == nullptr) {
			deferred().Reject(Napi::Error::New(env(), "SteamUser is not available.").Value());
			return;
		}

		uint32 ticketSize = 0;
		value->GetEncryptedAppTicket(nullptr, 0, &ticketSize);
		if (ticketSize == 0) {
			deferred().Reject(Napi::Error::New(env(), "Encrypted app ticket is not available.").Value());
			return;
		}

		std::vector<uint8_t> ticket(ticketSize);
		if (!value->GetEncryptedAppTicket(ticket.data(), static_cast<int>(ticket.size()), &ticketSize)) {
			deferred().Reject(Napi::Error::New(env(), "Error on reading encrypted app ticket.").Value());
			return;
		}

		ticket.resize(ticketSize);
		deferred().Resolve(Napi::Buffer<uint8_t>::Copy(env(), ticket.data(), ticket.size()));
	}
};

std::vector<std::unique_ptr<WebApiAuthTicketRequest>> webApiAuthTicketRequests;
std::vector<std::unique_ptr<EncryptedAppTicketRequest>> encryptedAppTicketRequests;

std::vector<uint8_t> userDataFromValue(Napi::Env env, Napi::Value value) {
	if (value.IsBuffer()) {
		Napi::Buffer<uint8_t> buffer = value.As<Napi::Buffer<uint8_t>>();
		return std::vector<uint8_t>(buffer.Data(), buffer.Data() + buffer.Length());
	}

	if (value.IsString()) {
		std::string string = value.As<Napi::String>();
		return std::vector<uint8_t>(string.begin(), string.end());
	}

	JS_THROW("userData must be a Buffer or string.");
	return {};
}
} // namespace

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

	std::vector<uint8_t> ticket(kAuthSessionTicketBufferSize);
	uint32 size = 0;
	HAuthTicket handle =
	    value->GetAuthSessionTicket(ticket.data(), static_cast<int>(ticket.size()), &size, nullptr);
	if (handle == k_HAuthTicketInvalid) {
		RET_NULL;
	}

	ticket.resize(std::min(static_cast<size_t>(size), ticket.size()));

	RET_VALUE(makeAuthSessionTicket(env, handle, ticket.data(), ticket.size()));
}

JS_METHOD(getAuthTicketForWebApi) {
	NAPI_ENV;

	const char *identityPtr = nullptr;
	std::string identity;
	if (info.Length() > 0 && !info[0].IsNull() && !info[0].IsUndefined()) {
		if (!info[0].IsString()) {
			JS_THROW("identity must be a string.");
			RET_UNDEFINED;
		}

		identity = info[0].As<Napi::String>();
		identityPtr = identity.c_str();
	}

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	cleanupCompletedCallResults(webApiAuthTicketRequests);
	HAuthTicket handle = value->GetAuthTicketForWebApi(identityPtr);
	if (handle == k_HAuthTicketInvalid) {
		RET_VALUE(rejectedPromise(env, "Steam returned an invalid Web API auth ticket handle."));
	}

	std::unique_ptr<WebApiAuthTicketRequest> request = std::make_unique<WebApiAuthTicketRequest>(env, handle);
	Napi::Promise promise = request->promise();
	webApiAuthTicketRequests.push_back(std::move(request));
	RET_VALUE(promise);
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

JS_METHOD(beginAuthSession) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "ticket")) {
		RET_UNDEFINED;
	}
	CSteamID steamId;
	if (!requireSteamId(env, info, 1, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	Napi::Buffer<uint8_t> ticket = info[0].As<Napi::Buffer<uint8_t>>();
	RET_NUM(
	    static_cast<int32_t>(
	        value->BeginAuthSession(ticket.Data(), static_cast<int>(ticket.Length()), steamId)
	    )
	);
}

JS_METHOD(endAuthSession) {
	NAPI_ENV;
	CSteamID steamId;
	if (!requireSteamId(env, info, 0, "steamId", &steamId)) {
		RET_UNDEFINED;
	}

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	value->EndAuthSession(steamId);
	RET_UNDEFINED;
}

JS_METHOD(requestEncryptedAppTicket) {
	NAPI_ENV;
	if (info.Length() < 1 || info[0].IsNull() || info[0].IsUndefined()) {
		JS_THROW("userData must be a Buffer or string.");
		RET_UNDEFINED;
	}

	std::vector<uint8_t> userData = userDataFromValue(env, info[0]);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->RequestEncryptedAppTicket(
	    userData.empty() ? nullptr : userData.data(), static_cast<int>(userData.size())
	);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam encrypted app ticket request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackCallResult<EncryptedAppTicketRequest>(env, call, encryptedAppTicketRequests));
}

JS_METHOD(getEncryptedAppTicket) {
	NAPI_ENV;
	ISteamUser *value = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 ticketSize = 0;
	value->GetEncryptedAppTicket(nullptr, 0, &ticketSize);
	if (ticketSize == 0) {
		RET_NULL;
	}

	std::vector<uint8_t> ticket(ticketSize);
	if (!value->GetEncryptedAppTicket(ticket.data(), static_cast<int>(ticket.size()), &ticketSize)) {
		RET_NULL;
	}

	ticket.resize(ticketSize);
	RET_VALUE(Napi::Buffer<uint8_t>::Copy(env, ticket.data(), ticket.size()));
}

JS_METHOD(decryptAppTicket) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "encryptedTicket")) {
		RET_UNDEFINED;
	}
	if (!requireBuffer(env, info, 1, "key")) {
		RET_UNDEFINED;
	}

	Napi::Buffer<uint8_t> encryptedTicket = info[0].As<Napi::Buffer<uint8_t>>();
	Napi::Buffer<uint8_t> key = info[1].As<Napi::Buffer<uint8_t>>();
	if (key.Length() != k_nSteamEncryptedAppTicketSymmetricKeyLen) {
		JS_THROW("key must be EncryptedAppTicketSymmetricKeyLength bytes.");
		RET_UNDEFINED;
	}

	std::vector<uint8_t> decryptedTicket(1024);
	uint32 decryptedTicketSize = static_cast<uint32>(decryptedTicket.size());
	bool success = SteamEncryptedAppTicket_BDecryptTicket(
	    encryptedTicket.Data(),
	    static_cast<uint32>(encryptedTicket.Length()),
	    decryptedTicket.data(),
	    &decryptedTicketSize,
	    key.Data(),
	    static_cast<int>(key.Length())
	);
	if (!success) {
		RET_NULL;
	}

	decryptedTicket.resize(decryptedTicketSize);
	RET_VALUE(Napi::Buffer<uint8_t>::Copy(env, decryptedTicket.data(), decryptedTicket.size()));
}

JS_METHOD(isTicketForApp) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "decryptedTicket")) {
		RET_UNDEFINED;
	}
	REQ_UINT32_ARG(1, appId);

	Napi::Buffer<uint8_t> decryptedTicket = info[0].As<Napi::Buffer<uint8_t>>();
	RET_BOOL(SteamEncryptedAppTicket_BIsTicketForApp(
	    decryptedTicket.Data(), static_cast<uint32>(decryptedTicket.Length()), appId
	));
}

JS_METHOD(getTicketIssueTime) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "decryptedTicket")) {
		RET_UNDEFINED;
	}

	Napi::Buffer<uint8_t> decryptedTicket = info[0].As<Napi::Buffer<uint8_t>>();
	RET_NUM(SteamEncryptedAppTicket_GetTicketIssueTime(
	    decryptedTicket.Data(), static_cast<uint32>(decryptedTicket.Length())
	));
}

JS_METHOD(getTicketSteamId) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "decryptedTicket")) {
		RET_UNDEFINED;
	}

	Napi::Buffer<uint8_t> decryptedTicket = info[0].As<Napi::Buffer<uint8_t>>();
	CSteamID steamId;
	SteamEncryptedAppTicket_GetTicketSteamID(
	    decryptedTicket.Data(), static_cast<uint32>(decryptedTicket.Length()), &steamId
	);
	RET_VALUE(jsStringFromUint64(env, steamId.ConvertToUint64()));
}

JS_METHOD(getTicketAppId) {
	NAPI_ENV;
	if (!requireBuffer(env, info, 0, "decryptedTicket")) {
		RET_UNDEFINED;
	}

	Napi::Buffer<uint8_t> decryptedTicket = info[0].As<Napi::Buffer<uint8_t>>();
	RET_NUM(SteamEncryptedAppTicket_GetTicketAppID(
	    decryptedTicket.Data(), static_cast<uint32>(decryptedTicket.Length())
	));
}

void handleGetTicketForWebApiResponse(GetTicketForWebApiResponse_t *param) {
	for (const std::unique_ptr<WebApiAuthTicketRequest> &request : webApiAuthTicketRequests) {
		if (request->handle() == param->m_hAuthTicket) {
			request->resolve(param);
		}
	}
	cleanupCompletedCallResults(webApiAuthTicketRequests);
}

void rejectPendingPromises(const std::string &message) {
	rejectPendingCallResults(webApiAuthTicketRequests, message);
	rejectPendingCallResults(encryptedAppTicketRequests, message);
}

void clearPendingPromises() {
	clearPendingCallResults(webApiAuthTicketRequests);
	clearPendingCallResults(encryptedAppTicketRequests);
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("getHSteamUser", Napi::Function::New(env, getHSteamUser));
	value.Set("isLoggedOn", Napi::Function::New(env, isLoggedOn));
	value.Set("getSteamId", Napi::Function::New(env, getSteamId));
	value.Set("getAuthSessionTicket", Napi::Function::New(env, getAuthSessionTicket));
	value.Set("getAuthTicketForWebApi", Napi::Function::New(env, getAuthTicketForWebApi));
	value.Set("cancelAuthTicket", Napi::Function::New(env, cancelAuthTicket));
	value.Set("beginAuthSession", Napi::Function::New(env, beginAuthSession));
	value.Set("endAuthSession", Napi::Function::New(env, endAuthSession));
	value.Set("requestEncryptedAppTicket", Napi::Function::New(env, requestEncryptedAppTicket));
	value.Set("getEncryptedAppTicket", Napi::Function::New(env, getEncryptedAppTicket));
	value.Set("decryptAppTicket", Napi::Function::New(env, decryptAppTicket));
	value.Set("isTicketForApp", Napi::Function::New(env, isTicketForApp));
	value.Set("getTicketIssueTime", Napi::Function::New(env, getTicketIssueTime));
	value.Set("getTicketSteamId", Napi::Function::New(env, getTicketSteamId));
	value.Set("getTicketAppId", Napi::Function::New(env, getTicketAppId));
	return value;
}
} // namespace steam_api::user
