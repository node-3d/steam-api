#include "ugc.hpp"

#include "call-result.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <system_error>
#include <utility>

namespace steam_api::ugc {
struct QueryOptions {
	uint32 appId;
	uint32 page;
};

struct Tags {
	bool isSet = false;
	std::vector<std::string> values;
	std::vector<const char *> pointers;
};

struct PublishOptions {
	uint32 appId;
	int32_t visibility;
	int32_t fileType;
	Tags tags;
};

struct UpdateOptions {
	bool hasVisibility = false;
	int32_t visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
	std::string changeDescription;
	Tags tags;
};

ISteamUGC *steamUgc(Napi::Env env) {
	ISteamUGC *value = SteamUGC();
	if (value == nullptr) {
		JS_THROW("SteamUGC is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamFriends *steamFriends(Napi::Env env) {
	ISteamFriends *value = SteamFriends();
	if (value == nullptr) {
		JS_THROW("SteamFriends is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamRemoteStorage *steamRemoteStorage(Napi::Env env) {
	ISteamRemoteStorage *value = SteamRemoteStorage();
	if (value == nullptr) {
		JS_THROW("SteamRemoteStorage is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamUser *steamUser(Napi::Env env) {
	ISteamUser *value = SteamUser();
	if (value == nullptr) {
		JS_THROW("SteamUser is not available. Call steam.initEx() first.");
	}
	return value;
}

ISteamUtils *steamUtils(Napi::Env env) {
	ISteamUtils *value = SteamUtils();
	if (value == nullptr) {
		JS_THROW("SteamUtils is not available. Call steam.initEx() first.");
	}
	return value;
}

bool numberToUint32(Napi::Env env, Napi::Value value, const char *name, uint32 *target) {
	if (!value.IsNumber()) {
		JS_THROW(std::string(name) + " must be a number.");
		return false;
	}

	double raw = value.As<Napi::Number>().DoubleValue();
	if (!std::isfinite(raw) || std::floor(raw) != raw || raw < 0.0 ||
	    raw > static_cast<double>(std::numeric_limits<uint32>::max())) {
		JS_THROW(std::string(name) + " must be a uint32 integer.");
		return false;
	}

	*target = static_cast<uint32>(raw);
	return true;
}

bool readOptionalUint32Property(
    Napi::Env env,
    Napi::Object object,
    const char *name,
    const char *alias,
    uint32 defaultValue,
    uint32 *target
) {
	Napi::Value value = object.Get(name);
	if ((value.IsNull() || value.IsUndefined()) && alias != nullptr) {
		value = object.Get(alias);
	}

	if (value.IsNull() || value.IsUndefined()) {
		*target = defaultValue;
		return true;
	}

	return numberToUint32(env, value, name, target);
}

bool readQueryOptions(Napi::Env env, Napi::Value value, QueryOptions *options) {
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		return false;
	}

	options->appId = utils->GetAppID();
	options->page = 1;

	if (value.IsNull() || value.IsUndefined()) {
		return true;
	}

	if (!value.IsObject()) {
		JS_THROW("UGC query options must be an object.");
		return false;
	}

	Napi::Object object = value.As<Napi::Object>();
	return readOptionalUint32Property(env, object, "appId", "app_id", options->appId, &options->appId) &&
	    readOptionalUint32Property(env, object, "page", "page_num", options->page, &options->page);
}

bool readOptionalInt32Property(
    Napi::Env env,
    Napi::Object object,
    const char *name,
    int32_t defaultValue,
    int32_t *target,
    bool *isSet = nullptr
) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		*target = defaultValue;
		if (isSet != nullptr) {
			*isSet = false;
		}
		return true;
	}

	if (!value.IsNumber()) {
		JS_THROW(std::string(name) + " must be a number.");
		return false;
	}

	double raw = value.As<Napi::Number>().DoubleValue();
	if (!std::isfinite(raw) || std::floor(raw) != raw ||
	    raw < static_cast<double>(std::numeric_limits<int32_t>::min()) ||
	    raw > static_cast<double>(std::numeric_limits<int32_t>::max())) {
		JS_THROW(std::string(name) + " must be an int32 integer.");
		return false;
	}

	*target = static_cast<int32_t>(raw);
	if (isSet != nullptr) {
		*isSet = true;
	}
	return true;
}

bool readOptionalStringProperty(Napi::Env env, Napi::Object object, const char *name, std::string *target) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		target->clear();
		return true;
	}
	if (!value.IsString()) {
		JS_THROW(std::string(name) + " must be a string.");
		return false;
	}

	*target = value.As<Napi::String>().Utf8Value();
	return true;
}

bool readOptionalTags(Napi::Env env, Napi::Object object, Tags *tags) {
	Napi::Value value = object.Get("tags");
	if (value.IsNull() || value.IsUndefined()) {
		tags->isSet = false;
		return true;
	}
	if (!value.IsArray()) {
		JS_THROW("tags must be an array of strings.");
		return false;
	}

	Napi::Array array = value.As<Napi::Array>();
	if (array.Length() > 100) {
		JS_THROW("tags must contain at most 100 strings.");
		return false;
	}

	tags->isSet = true;
	tags->values.clear();
	tags->pointers.clear();
	tags->values.reserve(array.Length());
	tags->pointers.reserve(array.Length());

	for (uint32 i = 0; i < array.Length(); i++) {
		Napi::Value item = array.Get(i);
		if (!item.IsString()) {
			JS_THROW("tags must contain only strings.");
			return false;
		}
		tags->values.push_back(item.As<Napi::String>().Utf8Value());
	}

	for (const std::string &tag : tags->values) {
		tags->pointers.push_back(tag.c_str());
	}

	return true;
}

bool readPublishOptions(Napi::Env env, Napi::Value value, PublishOptions *options) {
	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		return false;
	}

	options->appId = utils->GetAppID();
	options->visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
	options->fileType = k_EWorkshopFileTypeCommunity;

	if (value.IsNull() || value.IsUndefined()) {
		return true;
	}
	if (!value.IsObject()) {
		JS_THROW("workshop publish options must be an object.");
		return false;
	}

	Napi::Object object = value.As<Napi::Object>();
	return readOptionalUint32Property(env, object, "appId", "app_id", options->appId, &options->appId) &&
	    readOptionalInt32Property(env, object, "visibility", options->visibility, &options->visibility) &&
	    readOptionalInt32Property(env, object, "fileType", options->fileType, &options->fileType) &&
	    readOptionalTags(env, object, &options->tags);
}

bool readUpdateOptions(Napi::Env env, Napi::Value value, UpdateOptions *options) {
	if (value.IsNull() || value.IsUndefined()) {
		return true;
	}
	if (!value.IsObject()) {
		JS_THROW("workshop update options must be an object.");
		return false;
	}

	Napi::Object object = value.As<Napi::Object>();
	return readOptionalInt32Property(
	           env, object, "visibility", options->visibility, &options->visibility, &options->hasVisibility
	       ) &&
	    readOptionalStringProperty(env, object, "changeDescription", &options->changeDescription) &&
	    readOptionalTags(env, object, &options->tags);
}

SteamParamStringArray_t makeSteamTags(Tags &tags) {
	SteamParamStringArray_t steamTags = {};
	if (tags.isSet && tags.values.size() == 1 && tags.values.front().empty()) {
		steamTags.m_nNumStrings = 0;
		steamTags.m_ppStrings = nullptr;
		return steamTags;
	}

	steamTags.m_nNumStrings = static_cast<int32>(tags.pointers.size());
	steamTags.m_ppStrings = tags.pointers.empty() ? nullptr : tags.pointers.data();
	return steamTags;
}

std::string fileNameFromPath(const std::string &value) {
	size_t pos = value.find_last_of("/\\");
	if (pos == std::string::npos) {
		return value;
	}
	return value.substr(pos + 1);
}

bool writeFile(const std::filesystem::path &path, const std::vector<char> &content) {
	std::error_code error;
	std::filesystem::create_directories(path.parent_path(), error);
	if (error) {
		return false;
	}

	std::ofstream output(path, std::ios::binary);
	if (!output.is_open()) {
		return false;
	}

	output.write(content.data(), static_cast<std::streamsize>(content.size()));
	return output.good();
}

bool readFileContent(Napi::Env env, const std::string &path, std::vector<char> *content) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input.is_open()) {
		JS_THROW("Steam Cloud source file could not be opened: " + path);
		return false;
	}

	std::ifstream::pos_type fileSize = input.tellg();
	if (fileSize < 0 || fileSize > static_cast<std::ifstream::pos_type>(std::numeric_limits<int32>::max())) {
		JS_THROW("Steam Cloud source file is too large: " + path);
		return false;
	}

	content->resize(static_cast<size_t>(fileSize));
	input.seekg(0, std::ios::beg);
	if (!content->empty()) {
		input.read(content->data(), fileSize);
	}

	if (!input.good()) {
		JS_THROW("Steam Cloud source file could not be read: " + path);
		return false;
	}

	return true;
}

Napi::Object makeUgcDetails(Napi::Env env, const SteamUGCDetails_t &item) {
	Napi::Object result = JS_OBJECT;

	result.Set("acceptedForUse", item.m_bAcceptedForUse);
	result.Set("banned", item.m_bBanned);
	result.Set("tagsTruncated", item.m_bTagsTruncated);
	result.Set("fileType", static_cast<int32_t>(item.m_eFileType));
	result.Set("result", static_cast<int32_t>(item.m_eResult));
	result.Set("visibility", static_cast<int32_t>(item.m_eVisibility));
	result.Set("score", item.m_flScore);

	result.Set("file", jsStringFromUint64(env, item.m_hFile));
	result.Set("fileName", item.m_pchFileName);
	result.Set("fileSize", item.m_nFileSize);
	result.Set("previewFile", jsStringFromUint64(env, item.m_hPreviewFile));
	result.Set("previewFileSize", item.m_nPreviewFileSize);

	result.Set("steamIdOwner", jsStringFromUint64(env, item.m_ulSteamIDOwner));
	result.Set("consumerAppId", static_cast<uint32_t>(item.m_nConsumerAppID));
	result.Set("creatorAppId", static_cast<uint32_t>(item.m_nCreatorAppID));
	result.Set("publishedFileId", jsStringFromUint64(env, item.m_nPublishedFileId));

	result.Set("title", item.m_rgchTitle);
	result.Set("description", item.m_rgchDescription);
	result.Set("url", item.m_rgchURL);
	result.Set("tags", item.m_rgchTags);

	result.Set("timeAddedToUserList", item.m_rtimeAddedToUserList);
	result.Set("timeCreated", item.m_rtimeCreated);
	result.Set("timeUpdated", item.m_rtimeUpdated);
	result.Set("votesDown", item.m_unVotesDown);
	result.Set("votesUp", item.m_unVotesUp);
	result.Set("numChildren", item.m_unNumChildren);
	result.Set("totalFilesSize", jsStringFromUint64(env, item.m_ulTotalFilesSize));

	return result;
}

class QueryRequest : public PendingCallResultRequest<QueryRequest, SteamUGCQueryCompleted_t> {
	using Base = PendingCallResultRequest<QueryRequest, SteamUGCQueryCompleted_t>;

  public:
	explicit QueryRequest(Napi::Env env) : Base(env) {}

	void onCompleted(SteamUGCQueryCompleted_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		ISteamUGC *ugc = SteamUGC();
		if (ugc == nullptr) {
			deferred().Reject(Napi::Error::New(env(), "SteamUGC is not available.").Value());
			return;
		}

		if (ioFailure) {
			ugc->ReleaseQueryUGCRequest(result->m_handle);
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC query failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			ugc->ReleaseQueryUGCRequest(result->m_handle);
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC query failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Array items = Napi::Array::New(env(), result->m_unNumResultsReturned);
		for (uint32 i = 0; i < result->m_unNumResultsReturned; i++) {
			SteamUGCDetails_t item = {};
			if (!ugc->GetQueryUGCResult(result->m_handle, i, &item)) {
				ugc->ReleaseQueryUGCRequest(result->m_handle);
				deferred().Reject(
				    Napi::Error::New(env(), "Steam UGC query result could not be read.").Value()
				);
				return;
			}
			items.Set(i, makeUgcDetails(env(), item));
		}

		ugc->ReleaseQueryUGCRequest(result->m_handle);

		Napi::Object queryResult = Napi::Object::New(env());
		queryResult.Set("result", static_cast<int32_t>(result->m_eResult));
		queryResult.Set("totalMatchingResults", result->m_unTotalMatchingResults);
		queryResult.Set("cachedData", result->m_bCachedData);
		queryResult.Set("nextCursor", result->m_rgchNextCursor);
		queryResult.Set("items", items);
		deferred().Resolve(queryResult);
	}
};

std::vector<std::unique_ptr<QueryRequest>> pendingQueries;

Napi::Promise trackQuery(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<QueryRequest>(env, call, pendingQueries);
}

class DownloadRequest : public PendingCallResultRequest<DownloadRequest, RemoteStorageDownloadUGCResult_t> {
	using Base = PendingCallResultRequest<DownloadRequest, RemoteStorageDownloadUGCResult_t>;

  public:
	DownloadRequest(Napi::Env env, UGCHandle_t file, std::filesystem::path downloadDir)
	    : Base(env), _file(file), _downloadDir(std::move(downloadDir)) {}

	void onCompleted(RemoteStorageDownloadUGCResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		ISteamRemoteStorage *remoteStorage = SteamRemoteStorage();
		if (remoteStorage == nullptr) {
			deferred().Reject(Napi::Error::New(env(), "SteamRemoteStorage is not available.").Value());
			return;
		}

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC download failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC download failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		if (result->m_nSizeInBytes < 0) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC download returned an invalid file size.").Value()
			);
			return;
		}

		std::string fileName = fileNameFromPath(result->m_pchFileName);
		if (fileName.empty()) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC download returned an empty file name.").Value()
			);
			return;
		}

		std::vector<char> content(static_cast<size_t>(result->m_nSizeInBytes));
		int32 bytesRead = remoteStorage->UGCRead(
		    result->m_hFile, content.data(), result->m_nSizeInBytes, 0, k_EUGCRead_Close
		);
		if (bytesRead != result->m_nSizeInBytes) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC downloaded file could not be read.").Value()
			);
			return;
		}

		std::filesystem::path targetPath = _downloadDir / fileName;
		if (!writeFile(targetPath, content)) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC downloaded file could not be saved.").Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("file", jsStringFromUint64(env(), _file));
		value.Set("appId", static_cast<uint32_t>(result->m_nAppID));
		value.Set("sizeInBytes", result->m_nSizeInBytes);
		value.Set("fileName", fileName);
		value.Set("steamIdOwner", jsStringFromUint64(env(), result->m_ulSteamIDOwner));
		value.Set("path", targetPath.string());
		deferred().Resolve(value);
	}

  private:
	UGCHandle_t _file;
	std::filesystem::path _downloadDir;
};

std::vector<std::unique_ptr<DownloadRequest>> pendingDownloads;

Napi::Promise
trackDownload(Napi::Env env, SteamAPICall_t call, UGCHandle_t file, const std::string &downloadDir) {
	return trackCallResult<DownloadRequest>(
	    env, call, pendingDownloads, file, std::filesystem::path(downloadDir)
	);
}

class UnsubscribeRequest
    : public PendingCallResultRequest<UnsubscribeRequest, RemoteStorageUnsubscribePublishedFileResult_t> {
	using Base = PendingCallResultRequest<UnsubscribeRequest, RemoteStorageUnsubscribePublishedFileResult_t>;

  public:
	explicit UnsubscribeRequest(Napi::Env env) : Base(env) {}

	void onCompleted(RemoteStorageUnsubscribePublishedFileResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC unsubscribe failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC unsubscribe failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<UnsubscribeRequest>> pendingUnsubscribes;

Napi::Promise trackUnsubscribe(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<UnsubscribeRequest>(env, call, pendingUnsubscribes);
}

class FileShareRequest : public PendingCallResultRequest<FileShareRequest, RemoteStorageFileShareResult_t> {
	using Base = PendingCallResultRequest<FileShareRequest, RemoteStorageFileShareResult_t>;

  public:
	explicit FileShareRequest(Napi::Env env) : Base(env) {}

	void onCompleted(RemoteStorageFileShareResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC file share failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC file share failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("file", jsStringFromUint64(env(), result->m_hFile));
		value.Set("fileName", result->m_rgchFilename);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<FileShareRequest>> pendingFileShares;

Napi::Promise trackFileShare(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<FileShareRequest>(env, call, pendingFileShares);
}

class PublishWorkshopFileRequest
    : public PendingCallResultRequest<PublishWorkshopFileRequest, RemoteStoragePublishFileResult_t> {
	using Base = PendingCallResultRequest<PublishWorkshopFileRequest, RemoteStoragePublishFileResult_t>;

  public:
	explicit PublishWorkshopFileRequest(Napi::Env env) : Base(env) {}

	void onCompleted(RemoteStoragePublishFileResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam workshop publish failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam workshop publish failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		value.Set(
		    "userNeedsToAcceptWorkshopLegalAgreement", result->m_bUserNeedsToAcceptWorkshopLegalAgreement
		);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<PublishWorkshopFileRequest>> pendingWorkshopPublishes;

Napi::Promise trackWorkshopPublish(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<PublishWorkshopFileRequest>(env, call, pendingWorkshopPublishes);
}

class UpdatePublishedWorkshopFileRequest : public PendingCallResultRequest<
                                               UpdatePublishedWorkshopFileRequest,
                                               RemoteStorageUpdatePublishedFileResult_t> {
	using Base = PendingCallResultRequest<
	    UpdatePublishedWorkshopFileRequest,
	    RemoteStorageUpdatePublishedFileResult_t>;

  public:
	explicit UpdatePublishedWorkshopFileRequest(Napi::Env env) : Base(env) {}

	void onCompleted(RemoteStorageUpdatePublishedFileResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam workshop update failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam workshop update failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		value.Set(
		    "userNeedsToAcceptWorkshopLegalAgreement", result->m_bUserNeedsToAcceptWorkshopLegalAgreement
		);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<UpdatePublishedWorkshopFileRequest>> pendingWorkshopUpdates;

Napi::Promise trackWorkshopUpdate(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<UpdatePublishedWorkshopFileRequest>(env, call, pendingWorkshopUpdates);
}

void rejectPendingPromises(const std::string &message) {
	rejectPendingCallResults(pendingQueries, message);
	rejectPendingCallResults(pendingDownloads, message);
	rejectPendingCallResults(pendingUnsubscribes, message);
	rejectPendingCallResults(pendingFileShares, message);
	rejectPendingCallResults(pendingWorkshopPublishes, message);
	rejectPendingCallResults(pendingWorkshopUpdates, message);
}

void clearPendingPromises() {
	clearPendingCallResults(pendingQueries);
	clearPendingCallResults(pendingDownloads);
	clearPendingCallResults(pendingUnsubscribes);
	clearPendingCallResults(pendingFileShares);
	clearPendingCallResults(pendingWorkshopPublishes);
	clearPendingCallResults(pendingWorkshopUpdates);
}

JS_METHOD(showOverlay) {
	NAPI_ENV;

	std::string url;
	if (info.Length() == 0 || IS_ARG_EMPTY(0)) {
		ISteamUtils *utils = steamUtils(env);
		if (env.IsExceptionPending()) {
			RET_UNDEFINED;
		}
		url = "https://steamcommunity.com/app/" + std::to_string(utils->GetAppID()) + "/workshop/";
	} else {
		REQ_STR_ARG(0, publishedFileIdString);
		uint64 publishedFileId = 0;
		if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
			RET_UNDEFINED;
		}
		url = "https://steamcommunity.com/sharedfiles/filedetails/?id=" + std::to_string(publishedFileId);
	}

	ISteamFriends *friends = steamFriends(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	friends->ActivateGameOverlayToWebPage(url.c_str());
	RET_UNDEFINED;
}

JS_METHOD(getItems) {
	NAPI_ENV;
	REQ_INT32_ARG(1, matchingType);
	REQ_INT32_ARG(2, queryType);

	QueryOptions options = {};
	if (!readQueryOptions(env, info[0], &options)) {
		RET_UNDEFINED;
	}

	ISteamUGC *value = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	UGCQueryHandle_t queryHandle = value->CreateQueryAllUGCRequest(
	    static_cast<EUGCQuery>(queryType),
	    static_cast<EUGCMatchingUGCType>(matchingType),
	    k_uAppIdInvalid,
	    options.appId,
	    options.page
	);
	if (queryHandle == k_UGCQueryHandleInvalid) {
		JS_THROW("Steam UGC query handle could not be created.");
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->SendQueryUGCRequest(queryHandle);
	if (call == k_uAPICallInvalid) {
		value->ReleaseQueryUGCRequest(queryHandle);
		JS_THROW("Steam UGC query request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackQuery(env, call));
}

JS_METHOD(getUserItems) {
	NAPI_ENV;
	REQ_INT32_ARG(1, matchingType);
	REQ_INT32_ARG(2, sortOrder);
	REQ_INT32_ARG(3, list);

	QueryOptions options = {};
	if (!readQueryOptions(env, info[0], &options)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	ISteamUser *user = steamUser(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	UGCQueryHandle_t queryHandle = ugc->CreateQueryUserUGCRequest(
	    user->GetSteamID().GetAccountID(),
	    static_cast<EUserUGCList>(list),
	    static_cast<EUGCMatchingUGCType>(matchingType),
	    static_cast<EUserUGCListSortOrder>(sortOrder),
	    options.appId,
	    options.appId,
	    options.page
	);
	if (queryHandle == k_UGCQueryHandleInvalid) {
		JS_THROW("Steam UGC user query handle could not be created.");
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SendQueryUGCRequest(queryHandle);
	if (call == k_uAPICallInvalid) {
		ugc->ReleaseQueryUGCRequest(queryHandle);
		JS_THROW("Steam UGC user query request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackQuery(env, call));
}

JS_METHOD(downloadItem) {
	NAPI_ENV;
	REQ_STR_ARG(0, fileString);
	REQ_STR_ARG(1, downloadDir);

	if (downloadDir.empty()) {
		JS_THROW("downloadDir must be a non-empty string.");
		RET_UNDEFINED;
	}

	uint64 file = 0;
	if (!uint64FromJsString(env, fileString, "file", &file)) {
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = remoteStorage->UGCDownload(static_cast<UGCHandle_t>(file), 0);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC download request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackDownload(env, call, static_cast<UGCHandle_t>(file), downloadDir));
}

JS_METHOD(unsubscribe) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	uint64 publishedFileId = 0;
	if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call =
	    remoteStorage->UnsubscribePublishedFile(static_cast<PublishedFileId_t>(publishedFileId));
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC unsubscribe request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackUnsubscribe(env, call));
}

JS_METHOD(saveFilesToCloud) {
	NAPI_ENV;

	if (info.Length() < 1 || !info[0].IsArray()) {
		JS_THROW("filePaths must be an array of strings.");
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	Napi::Array filePaths = info[0].As<Napi::Array>();
	Napi::Array files = Napi::Array::New(env);
	uint32 fileCount = 0;

	for (uint32 i = 0; i < filePaths.Length(); i++) {
		Napi::Value item = filePaths.Get(i);
		if (!item.IsString()) {
			JS_THROW("filePaths must contain only strings.");
			RET_UNDEFINED;
		}

		std::string path = item.As<Napi::String>().Utf8Value();
		if (path.empty()) {
			continue;
		}

		std::string fileName = fileNameFromPath(path);
		if (fileName.empty()) {
			JS_THROW("filePaths entries must include file names.");
			RET_UNDEFINED;
		}

		std::vector<char> content;
		if (!readFileContent(env, path, &content)) {
			RET_UNDEFINED;
		}

		if (!remoteStorage->FileWrite(
		        fileName.c_str(),
		        content.empty() ? nullptr : content.data(),
		        static_cast<int32>(content.size())
		    )) {
			JS_THROW("Steam Cloud file could not be written: " + fileName);
			RET_UNDEFINED;
		}

		files.Set(fileCount, fileName);
		fileCount++;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("files", files);
	RET_VALUE(result);
}

JS_METHOD(fileShare) {
	NAPI_ENV;
	REQ_STR_ARG(0, filePath);

	std::string fileName = fileNameFromPath(filePath);
	if (fileName.empty()) {
		JS_THROW("filePath must include a file name.");
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = remoteStorage->FileShare(fileName.c_str());
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC file share request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackFileShare(env, call));
}

JS_METHOD(publishWorkshopFile) {
	NAPI_ENV;
	REQ_STR_ARG(1, filePath);
	REQ_STR_ARG(2, previewFilePath);
	REQ_STR_ARG(3, title);
	REQ_STR_ARG(4, description);

	PublishOptions options = {};
	if (!readPublishOptions(env, info[0], &options)) {
		RET_UNDEFINED;
	}

	std::string fileName = fileNameFromPath(filePath);
	if (fileName.empty()) {
		JS_THROW("filePath must include a file name.");
		RET_UNDEFINED;
	}

	std::string previewFileName = fileNameFromPath(previewFilePath);
	SteamParamStringArray_t tags = makeSteamTags(options.tags);

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = remoteStorage->PublishWorkshopFile(
	    fileName.c_str(),
	    previewFileName.empty() ? nullptr : previewFileName.c_str(),
	    options.appId,
	    title.c_str(),
	    description.empty() ? nullptr : description.c_str(),
	    static_cast<ERemoteStoragePublishedFileVisibility>(options.visibility),
	    &tags,
	    static_cast<EWorkshopFileType>(options.fileType)
	);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam workshop publish request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackWorkshopPublish(env, call));
}

JS_METHOD(updatePublishedWorkshopFile) {
	NAPI_ENV;
	REQ_STR_ARG(1, publishedFileIdString);
	REQ_STR_ARG(2, filePath);
	REQ_STR_ARG(3, previewFilePath);
	REQ_STR_ARG(4, title);
	REQ_STR_ARG(5, description);

	UpdateOptions options = {};
	if (!readUpdateOptions(env, info[0], &options)) {
		RET_UNDEFINED;
	}

	uint64 publishedFileId = 0;
	if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamRemoteStorage *remoteStorage = steamRemoteStorage(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	PublishedFileUpdateHandle_t updateHandle =
	    remoteStorage->CreatePublishedFileUpdateRequest(static_cast<PublishedFileId_t>(publishedFileId));
	if (updateHandle == k_PublishedFileUpdateHandleInvalid) {
		JS_THROW("Steam workshop update handle could not be created.");
		RET_UNDEFINED;
	}

	std::string fileName = fileNameFromPath(filePath);
	if (!fileName.empty() && !remoteStorage->UpdatePublishedFileFile(updateHandle, fileName.c_str())) {
		JS_THROW("Steam workshop update file could not be set.");
		RET_UNDEFINED;
	}

	std::string previewFileName = fileNameFromPath(previewFilePath);
	if (!previewFileName.empty() &&
	    !remoteStorage->UpdatePublishedFilePreviewFile(updateHandle, previewFileName.c_str())) {
		JS_THROW("Steam workshop update preview file could not be set.");
		RET_UNDEFINED;
	}

	if (!title.empty() && !remoteStorage->UpdatePublishedFileTitle(updateHandle, title.c_str())) {
		JS_THROW("Steam workshop update title could not be set.");
		RET_UNDEFINED;
	}

	if (!description.empty() &&
	    !remoteStorage->UpdatePublishedFileDescription(updateHandle, description.c_str())) {
		JS_THROW("Steam workshop update description could not be set.");
		RET_UNDEFINED;
	}

	if (options.hasVisibility &&
	    !remoteStorage->UpdatePublishedFileVisibility(
	        updateHandle, static_cast<ERemoteStoragePublishedFileVisibility>(options.visibility)
	    )) {
		JS_THROW("Steam workshop update visibility could not be set.");
		RET_UNDEFINED;
	}

	if (options.tags.isSet) {
		SteamParamStringArray_t tags = makeSteamTags(options.tags);
		if (!remoteStorage->UpdatePublishedFileTags(updateHandle, &tags)) {
			JS_THROW("Steam workshop update tags could not be set.");
			RET_UNDEFINED;
		}
	}

	if (!options.changeDescription.empty() &&
	    !remoteStorage->UpdatePublishedFileSetChangeDescription(
	        updateHandle, options.changeDescription.c_str()
	    )) {
		JS_THROW("Steam workshop update change description could not be set.");
		RET_UNDEFINED;
	}

	SteamAPICall_t call = remoteStorage->CommitPublishedFileUpdate(updateHandle);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam workshop update request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackWorkshopUpdate(env, call));
}

JS_METHOD(getItemState) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	uint64 publishedFileId = 0;
	if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *value = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(value->GetItemState(static_cast<PublishedFileId_t>(publishedFileId)));
}

JS_METHOD(getItemInstallInfo) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	uint64 publishedFileId = 0;
	if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *value = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint64 sizeOnDisk = 0;
	std::vector<char> folder(4096, '\0');
	uint32 timestamp = 0;
	bool ok = value->GetItemInstallInfo(
	    static_cast<PublishedFileId_t>(publishedFileId),
	    &sizeOnDisk,
	    folder.data(),
	    static_cast<uint32>(folder.size()),
	    &timestamp
	);

	if (!ok) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("sizeOnDisk", jsStringFromUint64(env, sizeOnDisk));
	result.Set("folder", folder.data());
	result.Set("timestamp", timestamp);
	RET_VALUE(result);
}

Napi::Object createNamespace(Napi::Env env) {
	Napi::Object value = JS_OBJECT;
	value.Set("showOverlay", Napi::Function::New(env, showOverlay));
	value.Set("getItems", Napi::Function::New(env, getItems));
	value.Set("getUserItems", Napi::Function::New(env, getUserItems));
	value.Set("downloadItem", Napi::Function::New(env, downloadItem));
	value.Set("unsubscribe", Napi::Function::New(env, unsubscribe));
	value.Set("saveFilesToCloud", Napi::Function::New(env, saveFilesToCloud));
	value.Set("fileShare", Napi::Function::New(env, fileShare));
	value.Set("publishWorkshopFile", Napi::Function::New(env, publishWorkshopFile));
	value.Set("updatePublishedWorkshopFile", Napi::Function::New(env, updatePublishedWorkshopFile));
	value.Set("getItemState", Napi::Function::New(env, getItemState));
	value.Set("getItemInstallInfo", Napi::Function::New(env, getItemInstallInfo));
	return value;
}
} // namespace steam_api::ugc
