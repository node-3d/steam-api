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
struct Tags {
	bool isSet = false;
	std::vector<std::string> values;
	std::vector<const char *> pointers;
};

struct KeyValueTag {
	std::string key;
	std::string value;
};

struct QueryOptions {
	uint32 appId;
	uint32 page;
	bool hasPage = false;
	bool hasCursor = false;
	std::string cursor;
	bool hasUserId = false;
	AccountID_t userId = 0;
	std::vector<std::string> requiredTags;
	std::vector<std::string> excludedTags;
	std::vector<Tags> requiredTagGroups;
	std::vector<KeyValueTag> requiredKeyValueTags;
	bool hasMatchAnyTag = false;
	bool matchAnyTag = false;
	bool hasAdminQuery = false;
	bool adminQuery = false;
	bool hasReturnOnlyIds = false;
	bool returnOnlyIds = false;
	bool hasReturnMetadata = false;
	bool returnMetadata = false;
	bool hasReturnLongDescription = false;
	bool returnLongDescription = false;
	bool hasReturnAdditionalPreviews = false;
	bool returnAdditionalPreviews = false;
	bool hasReturnChildren = false;
	bool returnChildren = false;
	bool hasReturnKeyValueTags = false;
	bool returnKeyValueTags = false;
	bool hasReturnPlaytimeStats = false;
	uint32 returnPlaytimeStatsDays = 0;
	bool hasReturnTotalOnly = false;
	bool returnTotalOnly = false;
	std::string cloudFileNameFilter;
	bool hasRankedByTrendDays = false;
	uint32 rankedByTrendDays = 0;
	std::string searchText;
	std::string language;
	bool hasAllowCachedResponse = false;
	uint32 allowCachedResponseMaxAgeSeconds = 0;
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
    uint32 *target,
    bool *isSet = nullptr
) {
	Napi::Value value = object.Get(name);
	if ((value.IsNull() || value.IsUndefined()) && alias != nullptr) {
		value = object.Get(alias);
	}

	if (value.IsNull() || value.IsUndefined()) {
		*target = defaultValue;
		if (isSet != nullptr) {
			*isSet = false;
		}
		return true;
	}

	if (!numberToUint32(env, value, name, target)) {
		return false;
	}
	if (isSet != nullptr) {
		*isSet = true;
	}
	return true;
}

bool readOptionalBoolProperty(
    Napi::Env env, Napi::Object object, const char *name, bool defaultValue, bool *target, bool *isSet
) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		*target = defaultValue;
		*isSet = false;
		return true;
	}

	if (!value.IsBoolean()) {
		JS_THROW(std::string(name) + " must be a boolean.");
		return false;
	}

	*target = value.As<Napi::Boolean>().Value();
	*isSet = true;
	return true;
}

bool readStringArray(Napi::Env env, Napi::Value value, const char *name, std::vector<std::string> *target) {
	if (!value.IsArray()) {
		JS_THROW(std::string(name) + " must be an array of strings.");
		return false;
	}

	Napi::Array array = value.As<Napi::Array>();
	target->clear();
	target->reserve(array.Length());

	for (uint32 i = 0; i < array.Length(); i++) {
		Napi::Value item = array.Get(i);
		if (!item.IsString()) {
			JS_THROW(std::string(name) + " must contain only strings.");
			return false;
		}
		target->push_back(item.As<Napi::String>().Utf8Value());
	}

	return true;
}

bool readOptionalStringArrayProperty(
    Napi::Env env, Napi::Object object, const char *name, std::vector<std::string> *target
) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		target->clear();
		return true;
	}

	return readStringArray(env, value, name, target);
}

bool readTagsFromArray(Napi::Env env, Napi::Value value, const char *name, Tags *tags) {
	if (!readStringArray(env, value, name, &tags->values)) {
		return false;
	}

	if (tags->values.size() > 100) {
		JS_THROW(std::string(name) + " must contain at most 100 strings.");
		return false;
	}

	tags->isSet = true;
	tags->pointers.clear();
	tags->pointers.reserve(tags->values.size());

	for (const std::string &tag : tags->values) {
		tags->pointers.push_back(tag.c_str());
	}

	return true;
}

bool readOptionalTagGroupsProperty(
    Napi::Env env, Napi::Object object, const char *name, std::vector<Tags> *target
) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		target->clear();
		return true;
	}

	if (!value.IsArray()) {
		JS_THROW(std::string(name) + " must be an array of string arrays.");
		return false;
	}

	Napi::Array array = value.As<Napi::Array>();
	target->clear();
	target->reserve(array.Length());

	for (uint32 i = 0; i < array.Length(); i++) {
		Tags group = {};
		std::string itemName = std::string(name) + " entries";
		if (!readTagsFromArray(env, array.Get(i), itemName.c_str(), &group)) {
			return false;
		}
		if (group.values.empty()) {
			JS_THROW(std::string(name) + " entries must contain at least one tag.");
			return false;
		}
		target->push_back(std::move(group));
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

bool readOptionalKeyValueTagsProperty(
    Napi::Env env, Napi::Object object, const char *name, std::vector<KeyValueTag> *target
) {
	Napi::Value value = object.Get(name);
	if (value.IsNull() || value.IsUndefined()) {
		target->clear();
		return true;
	}

	if (!value.IsArray()) {
		JS_THROW(std::string(name) + " must be an array of key-value tag objects.");
		return false;
	}

	Napi::Array array = value.As<Napi::Array>();
	target->clear();
	target->reserve(array.Length());

	for (uint32 i = 0; i < array.Length(); i++) {
		Napi::Value item = array.Get(i);
		if (!item.IsObject()) {
			JS_THROW(std::string(name) + " must contain only key-value tag objects.");
			return false;
		}

		Napi::Object tag = item.As<Napi::Object>();
		Napi::Value key = tag.Get("key");
		Napi::Value tagValue = tag.Get("value");
		if (!key.IsString()) {
			JS_THROW(std::string(name) + " entries must include a string key.");
			return false;
		}
		if (!tagValue.IsString()) {
			JS_THROW(std::string(name) + " entries must include a string value.");
			return false;
		}

		target->push_back({ key.As<Napi::String>().Utf8Value(), tagValue.As<Napi::String>().Utf8Value() });
	}

	return true;
}

bool readOptionalUserIdProperty(Napi::Env env, Napi::Object object, QueryOptions *options) {
	Napi::Value value = object.Get("userId");
	if (value.IsNull() || value.IsUndefined()) {
		options->hasUserId = false;
		return true;
	}
	if (!value.IsString()) {
		JS_THROW("userId must be a SteamID decimal string.");
		return false;
	}

	uint64 rawUserId = 0;
	if (!uint64FromJsString(env, value.As<Napi::String>().Utf8Value(), "userId", &rawUserId)) {
		return false;
	}

	CSteamID userId(rawUserId);
	if (!userId.IsValid() || userId.GetEAccountType() != k_EAccountTypeIndividual) {
		JS_THROW("userId must be a valid individual SteamID.");
		return false;
	}

	options->hasUserId = true;
	options->userId = userId.GetAccountID();
	return true;
}

bool readOptionalCursorProperty(Napi::Env env, Napi::Object object, QueryOptions *options) {
	Napi::Value value = object.Get("cursor");
	if (value.IsNull() || value.IsUndefined()) {
		options->hasCursor = false;
		options->cursor.clear();
		return true;
	}
	if (!value.IsString()) {
		JS_THROW("cursor must be a non-empty string.");
		return false;
	}

	options->cursor = value.As<Napi::String>().Utf8Value();
	if (options->cursor.empty()) {
		JS_THROW("cursor must be a non-empty string.");
		return false;
	}

	options->hasCursor = true;
	return true;
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
	    readOptionalUint32Property(
	           env, object, "page", "page_num", options->page, &options->page, &options->hasPage
	    ) &&
	    readOptionalCursorProperty(env, object, options) &&
	    readOptionalUserIdProperty(env, object, options) &&
	    readOptionalStringArrayProperty(env, object, "requiredTags", &options->requiredTags) &&
	    readOptionalStringArrayProperty(env, object, "excludedTags", &options->excludedTags) &&
	    readOptionalTagGroupsProperty(env, object, "requiredTagGroups", &options->requiredTagGroups) &&
	    readOptionalKeyValueTagsProperty(
	           env, object, "requiredKeyValueTags", &options->requiredKeyValueTags
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "matchAnyTag", false, &options->matchAnyTag, &options->hasMatchAnyTag
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "adminQuery", false, &options->adminQuery, &options->hasAdminQuery
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "returnOnlyIds", false, &options->returnOnlyIds, &options->hasReturnOnlyIds
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "returnMetadata", false, &options->returnMetadata, &options->hasReturnMetadata
	    ) &&
	    readOptionalBoolProperty(
	           env,
	           object,
	           "returnLongDescription",
	           false,
	           &options->returnLongDescription,
	           &options->hasReturnLongDescription
	    ) &&
	    readOptionalBoolProperty(
	           env,
	           object,
	           "returnAdditionalPreviews",
	           false,
	           &options->returnAdditionalPreviews,
	           &options->hasReturnAdditionalPreviews
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "returnChildren", false, &options->returnChildren, &options->hasReturnChildren
	    ) &&
	    readOptionalBoolProperty(
	           env,
	           object,
	           "returnKeyValueTags",
	           false,
	           &options->returnKeyValueTags,
	           &options->hasReturnKeyValueTags
	    ) &&
	    readOptionalUint32Property(
	           env,
	           object,
	           "returnPlaytimeStatsDays",
	           nullptr,
	           0,
	           &options->returnPlaytimeStatsDays,
	           &options->hasReturnPlaytimeStats
	    ) &&
	    readOptionalBoolProperty(
	           env, object, "returnTotalOnly", false, &options->returnTotalOnly, &options->hasReturnTotalOnly
	    ) &&
	    readOptionalStringProperty(env, object, "cloudFileNameFilter", &options->cloudFileNameFilter) &&
	    readOptionalUint32Property(
	           env,
	           object,
	           "rankedByTrendDays",
	           nullptr,
	           0,
	           &options->rankedByTrendDays,
	           &options->hasRankedByTrendDays
	    ) &&
	    readOptionalStringProperty(env, object, "searchText", &options->searchText) &&
	    readOptionalStringProperty(env, object, "language", &options->language) &&
	    readOptionalUint32Property(
	           env,
	           object,
	           "allowCachedResponseMaxAgeSeconds",
	           nullptr,
	           options->allowCachedResponseMaxAgeSeconds,
	           &options->allowCachedResponseMaxAgeSeconds,
	           &options->hasAllowCachedResponse
	    );
}

SteamParamStringArray_t makeSteamTags(Tags &tags);

bool applyQueryOptions(
    Napi::Env env, ISteamUGC *ugc, UGCQueryHandle_t queryHandle, QueryOptions &options, bool isAllUgcQuery
) {
	for (const std::string &tag : options.requiredTags) {
		if (!ugc->AddRequiredTag(queryHandle, tag.c_str())) {
			JS_THROW("Steam UGC query required tag could not be set.");
			return false;
		}
	}

	for (Tags &tagGroup : options.requiredTagGroups) {
		SteamParamStringArray_t steamTags = makeSteamTags(tagGroup);
		if (!ugc->AddRequiredTagGroup(queryHandle, &steamTags)) {
			JS_THROW("Steam UGC query required tag group could not be set.");
			return false;
		}
	}

	for (const std::string &tag : options.excludedTags) {
		if (!ugc->AddExcludedTag(queryHandle, tag.c_str())) {
			JS_THROW("Steam UGC query excluded tag could not be set.");
			return false;
		}
	}

	for (const KeyValueTag &tag : options.requiredKeyValueTags) {
		if (!ugc->AddRequiredKeyValueTag(queryHandle, tag.key.c_str(), tag.value.c_str())) {
			JS_THROW("Steam UGC query required key-value tag could not be set.");
			return false;
		}
	}

	if (options.hasMatchAnyTag) {
		if (!isAllUgcQuery) {
			JS_THROW("matchAnyTag is only supported by ugc.getItems().");
			return false;
		}
		if (!ugc->SetMatchAnyTag(queryHandle, options.matchAnyTag)) {
			JS_THROW("Steam UGC query match-any-tag option could not be set.");
			return false;
		}
	}

	if (options.hasAdminQuery && !ugc->SetAdminQuery(queryHandle, options.adminQuery)) {
		JS_THROW("Steam UGC query admin option could not be set.");
		return false;
	}

	if (options.hasReturnOnlyIds && !ugc->SetReturnOnlyIDs(queryHandle, options.returnOnlyIds)) {
		JS_THROW("Steam UGC query return-only-ids option could not be set.");
		return false;
	}

	if (options.hasReturnMetadata && !ugc->SetReturnMetadata(queryHandle, options.returnMetadata)) {
		JS_THROW("Steam UGC query metadata return option could not be set.");
		return false;
	}

	if (options.hasReturnLongDescription &&
	    !ugc->SetReturnLongDescription(queryHandle, options.returnLongDescription)) {
		JS_THROW("Steam UGC query long-description return option could not be set.");
		return false;
	}

	if (options.hasReturnAdditionalPreviews &&
	    !ugc->SetReturnAdditionalPreviews(queryHandle, options.returnAdditionalPreviews)) {
		JS_THROW("Steam UGC query additional-previews return option could not be set.");
		return false;
	}

	if (options.hasReturnChildren && !ugc->SetReturnChildren(queryHandle, options.returnChildren)) {
		JS_THROW("Steam UGC query children return option could not be set.");
		return false;
	}

	if (options.hasReturnKeyValueTags &&
	    !ugc->SetReturnKeyValueTags(queryHandle, options.returnKeyValueTags)) {
		JS_THROW("Steam UGC query key-value-tags return option could not be set.");
		return false;
	}

	if (options.hasReturnPlaytimeStats) {
		if (options.returnPlaytimeStatsDays == 0) {
			JS_THROW("returnPlaytimeStatsDays must be greater than 0.");
			return false;
		}
		if (!ugc->SetReturnPlaytimeStats(queryHandle, options.returnPlaytimeStatsDays)) {
			JS_THROW("Steam UGC query playtime-stats return option could not be set.");
			return false;
		}
	}

	if (options.hasReturnTotalOnly && !ugc->SetReturnTotalOnly(queryHandle, options.returnTotalOnly)) {
		JS_THROW("Steam UGC query total-only return option could not be set.");
		return false;
	}

	if (!options.cloudFileNameFilter.empty()) {
		if (isAllUgcQuery) {
			JS_THROW("cloudFileNameFilter is only supported by ugc.getUserItems().");
			return false;
		}
		if (!ugc->SetCloudFileNameFilter(queryHandle, options.cloudFileNameFilter.c_str())) {
			JS_THROW("Steam UGC query cloud-file-name filter could not be set.");
			return false;
		}
	}

	if (options.hasRankedByTrendDays) {
		if (!isAllUgcQuery) {
			JS_THROW("rankedByTrendDays is only supported by ugc.getItems().");
			return false;
		}
		if (options.rankedByTrendDays < 1 || options.rankedByTrendDays > 365) {
			JS_THROW("rankedByTrendDays must be between 1 and 365.");
			return false;
		}
		if (!ugc->SetRankedByTrendDays(queryHandle, options.rankedByTrendDays)) {
			JS_THROW("Steam UGC query ranked-by-trend-days option could not be set.");
			return false;
		}
	}

	if (!options.searchText.empty()) {
		if (!isAllUgcQuery) {
			JS_THROW("searchText is only supported by ugc.getItems().");
			return false;
		}
		if (!ugc->SetSearchText(queryHandle, options.searchText.c_str())) {
			JS_THROW("Steam UGC query search text could not be set.");
			return false;
		}
	}

	if (!options.language.empty() && !ugc->SetLanguage(queryHandle, options.language.c_str())) {
		JS_THROW("Steam UGC query language option could not be set.");
		return false;
	}

	if (options.hasAllowCachedResponse &&
	    !ugc->SetAllowCachedResponse(queryHandle, options.allowCachedResponseMaxAgeSeconds)) {
		JS_THROW("Steam UGC query cached-response option could not be set.");
		return false;
	}

	return true;
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

bool readTagsArray(Napi::Env env, Napi::Value value, Tags *tags) {
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

bool addQueryReturnFields(
    Napi::Env env,
    ISteamUGC *ugc,
    UGCQueryHandle_t queryHandle,
    uint32 index,
    const SteamUGCDetails_t &item,
    const QueryOptions &options,
    Napi::Object result,
    std::string *error
) {
	if (options.hasReturnMetadata && options.returnMetadata) {
		std::vector<char> metadata(k_cchDeveloperMetadataMax, '\0');
		if (!ugc->GetQueryUGCMetadata(
		        queryHandle, index, metadata.data(), static_cast<uint32>(metadata.size())
		    )) {
			*error = "Steam UGC query metadata could not be read.";
			return false;
		}
		result.Set("metadata", metadata.data());
	}

	if (options.hasReturnChildren && options.returnChildren) {
		std::vector<PublishedFileId_t> children(item.m_unNumChildren);
		if (!children.empty() &&
		    !ugc->GetQueryUGCChildren(
		        queryHandle, index, children.data(), static_cast<uint32>(children.size())
		    )) {
			*error = "Steam UGC query children could not be read.";
			return false;
		}

		Napi::Array value = Napi::Array::New(env, children.size());
		for (uint32 i = 0; i < children.size(); i++) {
			value.Set(i, jsStringFromUint64(env, children[i]));
		}
		result.Set("children", value);
	}

	if (options.hasReturnAdditionalPreviews && options.returnAdditionalPreviews) {
		uint32 count = ugc->GetQueryUGCNumAdditionalPreviews(queryHandle, index);
		Napi::Array value = Napi::Array::New(env, count);

		for (uint32 i = 0; i < count; i++) {
			std::vector<char> urlOrVideoId(k_cchPublishedFileURLMax, '\0');
			std::vector<char> originalFileName(k_cchFilenameMax, '\0');
			EItemPreviewType previewType = k_EItemPreviewType_Image;
			if (!ugc->GetQueryUGCAdditionalPreview(
			        queryHandle,
			        index,
			        i,
			        urlOrVideoId.data(),
			        static_cast<uint32>(urlOrVideoId.size()),
			        originalFileName.data(),
			        static_cast<uint32>(originalFileName.size()),
			        &previewType
			    )) {
				*error = "Steam UGC query additional preview could not be read.";
				return false;
			}

			Napi::Object preview = JS_OBJECT;
			preview.Set("urlOrVideoId", urlOrVideoId.data());
			preview.Set("originalFileName", originalFileName.data());
			preview.Set("previewType", static_cast<int32_t>(previewType));
			value.Set(i, preview);
		}
		result.Set("additionalPreviews", value);
	}

	if (options.hasReturnKeyValueTags && options.returnKeyValueTags) {
		uint32 count = ugc->GetQueryUGCNumKeyValueTags(queryHandle, index);
		Napi::Array value = Napi::Array::New(env, count);

		for (uint32 i = 0; i < count; i++) {
			std::vector<char> key(k_cchTagListMax, '\0');
			std::vector<char> tagValue(k_cchTagListMax, '\0');
			if (!ugc->GetQueryUGCKeyValueTag(
			        queryHandle,
			        index,
			        i,
			        key.data(),
			        static_cast<uint32>(key.size()),
			        tagValue.data(),
			        static_cast<uint32>(tagValue.size())
			    )) {
				*error = "Steam UGC query key-value tag could not be read.";
				return false;
			}

			Napi::Object tag = JS_OBJECT;
			tag.Set("key", key.data());
			tag.Set("value", tagValue.data());
			value.Set(i, tag);
		}
		result.Set("keyValueTags", value);
	}

	if (options.hasReturnPlaytimeStats) {
		uint64 secondsPlayed = 0;
		uint64 playtimeSessions = 0;
		uint64 secondsPlayedDuringTimePeriod = 0;
		uint64 playtimeSessionsDuringTimePeriod = 0;

		if (!ugc->GetQueryUGCStatistic(
		        queryHandle, index, k_EItemStatistic_NumSecondsPlayed, &secondsPlayed
		    ) ||
		    !ugc->GetQueryUGCStatistic(
		        queryHandle, index, k_EItemStatistic_NumPlaytimeSessions, &playtimeSessions
		    ) ||
		    !ugc->GetQueryUGCStatistic(
		        queryHandle,
		        index,
		        k_EItemStatistic_NumSecondsPlayedDuringTimePeriod,
		        &secondsPlayedDuringTimePeriod
		    ) ||
		    !ugc->GetQueryUGCStatistic(
		        queryHandle,
		        index,
		        k_EItemStatistic_NumPlaytimeSessionsDuringTimePeriod,
		        &playtimeSessionsDuringTimePeriod
		    )) {
			*error = "Steam UGC query playtime stats could not be read.";
			return false;
		}

		Napi::Object value = JS_OBJECT;
		value.Set("secondsPlayed", jsStringFromUint64(env, secondsPlayed));
		value.Set("playtimeSessions", jsStringFromUint64(env, playtimeSessions));
		value.Set("secondsPlayedDuringTimePeriod", jsStringFromUint64(env, secondsPlayedDuringTimePeriod));
		value.Set(
		    "playtimeSessionsDuringTimePeriod", jsStringFromUint64(env, playtimeSessionsDuringTimePeriod)
		);
		result.Set("playtimeStats", value);
	}

	return true;
}

bool readUgcUpdateHandle(Napi::Env env, const std::string &source, UGCUpdateHandle_t *target) {
	uint64 value = 0;
	if (!uint64FromJsString(env, source, "updateHandle", &value)) {
		return false;
	}
	*target = static_cast<UGCUpdateHandle_t>(value);
	return true;
}

bool readPublishedFileId(Napi::Env env, const std::string &source, PublishedFileId_t *target) {
	uint64 value = 0;
	if (!uint64FromJsString(env, source, "publishedFileId", &value)) {
		return false;
	}
	*target = static_cast<PublishedFileId_t>(value);
	return true;
}

class QueryRequest : public PendingCallResultRequest<QueryRequest, SteamUGCQueryCompleted_t> {
	using Base = PendingCallResultRequest<QueryRequest, SteamUGCQueryCompleted_t>;

  public:
	QueryRequest(Napi::Env env, QueryOptions options) : Base(env), _options(std::move(options)) {}

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

			Napi::Object value = makeUgcDetails(env(), item);
			std::string error;
			if (!addQueryReturnFields(env(), ugc, result->m_handle, i, item, _options, value, &error)) {
				ugc->ReleaseQueryUGCRequest(result->m_handle);
				deferred().Reject(Napi::Error::New(env(), error).Value());
				return;
			}
			items.Set(i, value);
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

  private:
	QueryOptions _options;
};

std::vector<std::unique_ptr<QueryRequest>> pendingQueries;

Napi::Promise trackQuery(Napi::Env env, SteamAPICall_t call, QueryOptions options) {
	return trackCallResult<QueryRequest>(env, call, pendingQueries, std::move(options));
}

class CreateItemRequest : public PendingCallResultRequest<CreateItemRequest, CreateItemResult_t> {
	using Base = PendingCallResultRequest<CreateItemRequest, CreateItemResult_t>;

  public:
	explicit CreateItemRequest(Napi::Env env) : Base(env) {}

	void onCompleted(CreateItemResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC create item failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC create item failed with result " +
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

std::vector<std::unique_ptr<CreateItemRequest>> pendingCreateItems;

Napi::Promise trackCreateItem(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<CreateItemRequest>(env, call, pendingCreateItems);
}

class SubmitItemUpdateRequest
    : public PendingCallResultRequest<SubmitItemUpdateRequest, SubmitItemUpdateResult_t> {
	using Base = PendingCallResultRequest<SubmitItemUpdateRequest, SubmitItemUpdateResult_t>;

  public:
	explicit SubmitItemUpdateRequest(Napi::Env env) : Base(env) {}

	void onCompleted(SubmitItemUpdateResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC submit item update failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC submit item update failed with result " +
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

std::vector<std::unique_ptr<SubmitItemUpdateRequest>> pendingSubmitItemUpdates;

Napi::Promise trackSubmitItemUpdate(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<SubmitItemUpdateRequest>(env, call, pendingSubmitItemUpdates);
}

class SetUserItemVoteRequest
    : public PendingCallResultRequest<SetUserItemVoteRequest, SetUserItemVoteResult_t> {
	using Base = PendingCallResultRequest<SetUserItemVoteRequest, SetUserItemVoteResult_t>;

  public:
	explicit SetUserItemVoteRequest(Napi::Env env) : Base(env) {}

	void onCompleted(SetUserItemVoteResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC set user item vote failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC set user item vote failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		value.Set("voteUp", result->m_bVoteUp);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<SetUserItemVoteRequest>> pendingSetUserItemVotes;

Napi::Promise trackSetUserItemVote(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<SetUserItemVoteRequest>(env, call, pendingSetUserItemVotes);
}

class GetUserItemVoteRequest
    : public PendingCallResultRequest<GetUserItemVoteRequest, GetUserItemVoteResult_t> {
	using Base = PendingCallResultRequest<GetUserItemVoteRequest, GetUserItemVoteResult_t>;

  public:
	explicit GetUserItemVoteRequest(Napi::Env env) : Base(env) {}

	void onCompleted(GetUserItemVoteResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC get user item vote failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC get user item vote failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		value.Set("votedUp", result->m_bVotedUp);
		value.Set("votedDown", result->m_bVotedDown);
		value.Set("voteSkipped", result->m_bVoteSkipped);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<GetUserItemVoteRequest>> pendingGetUserItemVotes;

Napi::Promise trackGetUserItemVote(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<GetUserItemVoteRequest>(env, call, pendingGetUserItemVotes);
}

class FavoriteItemsListChangedRequest
    : public PendingCallResultRequest<FavoriteItemsListChangedRequest, UserFavoriteItemsListChanged_t> {
	using Base = PendingCallResultRequest<FavoriteItemsListChangedRequest, UserFavoriteItemsListChanged_t>;

  public:
	explicit FavoriteItemsListChangedRequest(Napi::Env env) : Base(env) {}

	void onCompleted(UserFavoriteItemsListChanged_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC favorite item request failed: Steam API IO failure.")
			        .Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC favorite item request failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(env(), result->m_nPublishedFileId));
		value.Set("wasAddRequest", result->m_bWasAddRequest);
		deferred().Resolve(value);
	}
};

std::vector<std::unique_ptr<FavoriteItemsListChangedRequest>> pendingFavoriteItemsListChanges;

Napi::Promise trackFavoriteItemsListChanged(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<FavoriteItemsListChangedRequest>(env, call, pendingFavoriteItemsListChanges);
}

template <typename TResult>
class DependencyRequest : public PendingCallResultRequest<DependencyRequest<TResult>, TResult> {
	using Base = PendingCallResultRequest<DependencyRequest<TResult>, TResult>;

  public:
	explicit DependencyRequest(Napi::Env env) : Base(env) {}

	void onCompleted(TResult *result, bool ioFailure) {
		this->markCompleted();
		Napi::HandleScope scope(this->env());

		if (ioFailure) {
			this->deferred().Reject(
			    Napi::Error::New(this->env(), "Steam UGC dependency request failed: Steam API IO failure.")
			        .Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			this->deferred().Reject(
			    Napi::Error::New(
			        this->env(),
			        "Steam UGC dependency request failed with result " +
			            std::to_string(static_cast<int32_t>(result->m_eResult)) + "."
			    )
			        .Value()
			);
			return;
		}

		Napi::Object value = Napi::Object::New(this->env());
		value.Set("result", static_cast<int32_t>(result->m_eResult));
		value.Set("publishedFileId", jsStringFromUint64(this->env(), result->m_nPublishedFileId));
		value.Set("childPublishedFileId", jsStringFromUint64(this->env(), result->m_nChildPublishedFileId));
		this->deferred().Resolve(value);
	}
};

using AddDependencyRequest = DependencyRequest<AddUGCDependencyResult_t>;
using RemoveDependencyRequest = DependencyRequest<RemoveUGCDependencyResult_t>;

std::vector<std::unique_ptr<AddDependencyRequest>> pendingAddDependencies;
std::vector<std::unique_ptr<RemoveDependencyRequest>> pendingRemoveDependencies;

Napi::Promise trackAddDependency(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<AddDependencyRequest>(env, call, pendingAddDependencies);
}

Napi::Promise trackRemoveDependency(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<RemoveDependencyRequest>(env, call, pendingRemoveDependencies);
}

class SubscribeItemRequest
    : public PendingCallResultRequest<SubscribeItemRequest, RemoteStorageSubscribePublishedFileResult_t> {
	using Base = PendingCallResultRequest<SubscribeItemRequest, RemoteStorageSubscribePublishedFileResult_t>;

  public:
	explicit SubscribeItemRequest(Napi::Env env) : Base(env) {}

	void onCompleted(RemoteStorageSubscribePublishedFileResult_t *result, bool ioFailure) {
		markCompleted();
		Napi::HandleScope scope(env());

		if (ioFailure) {
			deferred().Reject(
			    Napi::Error::New(env(), "Steam UGC subscribe item failed: Steam API IO failure.").Value()
			);
			return;
		}

		if (result->m_eResult != k_EResultOK) {
			deferred().Reject(
			    Napi::Error::New(
			        env(),
			        "Steam UGC subscribe item failed with result " +
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

std::vector<std::unique_ptr<SubscribeItemRequest>> pendingSubscribeItems;

Napi::Promise trackSubscribeItem(Napi::Env env, SteamAPICall_t call) {
	return trackCallResult<SubscribeItemRequest>(env, call, pendingSubscribeItems);
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
	rejectPendingCallResults(pendingCreateItems, message);
	rejectPendingCallResults(pendingSubmitItemUpdates, message);
	rejectPendingCallResults(pendingSetUserItemVotes, message);
	rejectPendingCallResults(pendingGetUserItemVotes, message);
	rejectPendingCallResults(pendingFavoriteItemsListChanges, message);
	rejectPendingCallResults(pendingAddDependencies, message);
	rejectPendingCallResults(pendingRemoveDependencies, message);
	rejectPendingCallResults(pendingSubscribeItems, message);
	rejectPendingCallResults(pendingDownloads, message);
	rejectPendingCallResults(pendingUnsubscribes, message);
	rejectPendingCallResults(pendingFileShares, message);
	rejectPendingCallResults(pendingWorkshopPublishes, message);
	rejectPendingCallResults(pendingWorkshopUpdates, message);
}

void clearPendingPromises() {
	clearPendingCallResults(pendingQueries);
	clearPendingCallResults(pendingCreateItems);
	clearPendingCallResults(pendingSubmitItemUpdates);
	clearPendingCallResults(pendingSetUserItemVotes);
	clearPendingCallResults(pendingGetUserItemVotes);
	clearPendingCallResults(pendingFavoriteItemsListChanges);
	clearPendingCallResults(pendingAddDependencies);
	clearPendingCallResults(pendingRemoveDependencies);
	clearPendingCallResults(pendingSubscribeItems);
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
	if (options.hasCursor && options.hasPage) {
		JS_THROW("cursor cannot be combined with page.");
		RET_UNDEFINED;
	}
	if (options.hasUserId) {
		JS_THROW("userId is only supported by ugc.getUserItems().");
		RET_UNDEFINED;
	}

	ISteamUGC *value = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	UGCQueryHandle_t queryHandle = k_UGCQueryHandleInvalid;
	if (options.hasCursor) {
		queryHandle = value->CreateQueryAllUGCRequest(
		    static_cast<EUGCQuery>(queryType),
		    static_cast<EUGCMatchingUGCType>(matchingType),
		    k_uAppIdInvalid,
		    options.appId,
		    options.cursor.c_str()
		);
	} else {
		queryHandle = value->CreateQueryAllUGCRequest(
		    static_cast<EUGCQuery>(queryType),
		    static_cast<EUGCMatchingUGCType>(matchingType),
		    k_uAppIdInvalid,
		    options.appId,
		    options.page
		);
	}
	if (queryHandle == k_UGCQueryHandleInvalid) {
		JS_THROW("Steam UGC query handle could not be created.");
		RET_UNDEFINED;
	}

	if (!applyQueryOptions(env, value, queryHandle, options, true)) {
		value->ReleaseQueryUGCRequest(queryHandle);
		RET_UNDEFINED;
	}

	SteamAPICall_t call = value->SendQueryUGCRequest(queryHandle);
	if (call == k_uAPICallInvalid) {
		value->ReleaseQueryUGCRequest(queryHandle);
		JS_THROW("Steam UGC query request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackQuery(env, call, std::move(options)));
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
	if (options.hasCursor) {
		JS_THROW("cursor is only supported by ugc.getItems().");
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	AccountID_t userId = options.userId;
	if (!options.hasUserId) {
		ISteamUser *user = steamUser(env);
		if (env.IsExceptionPending()) {
			RET_UNDEFINED;
		}
		userId = user->GetSteamID().GetAccountID();
	}

	UGCQueryHandle_t queryHandle = ugc->CreateQueryUserUGCRequest(
	    userId,
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

	if (!applyQueryOptions(env, ugc, queryHandle, options, false)) {
		ugc->ReleaseQueryUGCRequest(queryHandle);
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SendQueryUGCRequest(queryHandle);
	if (call == k_uAPICallInvalid) {
		ugc->ReleaseQueryUGCRequest(queryHandle);
		JS_THROW("Steam UGC user query request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackQuery(env, call, std::move(options)));
}

JS_METHOD(getItemsByIds) {
	NAPI_ENV;
	REQ_ARRAY_ARG(1, publishedFileIds);

	QueryOptions options = {};
	if (!readQueryOptions(env, info[0], &options)) {
		RET_UNDEFINED;
	}
	if (options.hasUserId) {
		JS_THROW("userId is only supported by ugc.getUserItems().");
		RET_UNDEFINED;
	}
	if (options.hasCursor) {
		JS_THROW("cursor is only supported by ugc.getItems().");
		RET_UNDEFINED;
	}
	if (!options.cloudFileNameFilter.empty()) {
		JS_THROW("cloudFileNameFilter is only supported by ugc.getUserItems().");
		RET_UNDEFINED;
	}

	if (publishedFileIds.Length() == 0) {
		JS_THROW("publishedFileIds must contain at least one item ID.");
		RET_UNDEFINED;
	}

	std::vector<PublishedFileId_t> ids;
	ids.reserve(publishedFileIds.Length());
	for (uint32 i = 0; i < publishedFileIds.Length(); i++) {
		Napi::Value item = publishedFileIds.Get(i);
		if (!item.IsString()) {
			JS_THROW("publishedFileIds must contain only published file ID decimal strings.");
			RET_UNDEFINED;
		}
		PublishedFileId_t id = 0;
		if (!readPublishedFileId(env, item.As<Napi::String>().Utf8Value(), &id)) {
			RET_UNDEFINED;
		}
		ids.push_back(id);
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	UGCQueryHandle_t queryHandle =
	    ugc->CreateQueryUGCDetailsRequest(ids.data(), static_cast<uint32>(ids.size()));
	if (queryHandle == k_UGCQueryHandleInvalid) {
		JS_THROW("Steam UGC details query handle could not be created.");
		RET_UNDEFINED;
	}

	if (!applyQueryOptions(env, ugc, queryHandle, options, false)) {
		ugc->ReleaseQueryUGCRequest(queryHandle);
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SendQueryUGCRequest(queryHandle);
	if (call == k_uAPICallInvalid) {
		ugc->ReleaseQueryUGCRequest(queryHandle);
		JS_THROW("Steam UGC details query request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackQuery(env, call, std::move(options)));
}

JS_METHOD(createItem) {
	NAPI_ENV;

	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 appId = utils->GetAppID();
	int32_t fileType = k_EWorkshopFileTypeCommunity;

	if (info.Length() > 0 && !IS_ARG_EMPTY(0) && !numberToUint32(env, info[0], "appId", &appId)) {
		RET_UNDEFINED;
	}
	if (info.Length() > 1 && !IS_ARG_EMPTY(1)) {
		if (!info[1].IsNumber()) {
			JS_THROW("fileType must be a number.");
			RET_UNDEFINED;
		}
		fileType = info[1].As<Napi::Number>().Int32Value();
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->CreateItem(appId, static_cast<EWorkshopFileType>(fileType));
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC create item request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackCreateItem(env, call));
}

JS_METHOD(startItemUpdate) {
	NAPI_ENV;
	REQ_UINT32_ARG(0, appId);
	REQ_STR_ARG(1, publishedFileIdString);

	uint64 publishedFileId = 0;
	if (!uint64FromJsString(env, publishedFileIdString, "publishedFileId", &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	UGCUpdateHandle_t handle = ugc->StartItemUpdate(appId, static_cast<PublishedFileId_t>(publishedFileId));
	if (handle == k_UGCUpdateHandleInvalid) {
		JS_THROW("Steam UGC item update handle could not be created.");
		RET_UNDEFINED;
	}

	RET_VALUE(jsStringFromUint64(env, handle));
}

JS_METHOD(setItemTitle) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, title);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemTitle(updateHandle, title.c_str()));
}

JS_METHOD(setItemDescription) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, description);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemDescription(updateHandle, description.c_str()));
}

JS_METHOD(setItemMetadata) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, metadata);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemMetadata(updateHandle, metadata.c_str()));
}

JS_METHOD(setItemVisibility) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_INT32_ARG(1, visibility);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(
	    ugc->SetItemVisibility(updateHandle, static_cast<ERemoteStoragePublishedFileVisibility>(visibility))
	);
}

JS_METHOD(setItemTags) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	USE_BOOL_ARG(2, allowAdminTags, false);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	Tags tags = {};
	if (info.Length() < 2 || !readTagsArray(env, info[1], &tags)) {
		RET_UNDEFINED;
	}
	SteamParamStringArray_t steamTags = makeSteamTags(tags);

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemTags(updateHandle, &steamTags, allowAdminTags));
}

JS_METHOD(removeAllItemKeyValueTags) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->RemoveAllItemKeyValueTags(updateHandle));
}

JS_METHOD(removeItemKeyValueTags) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, key);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->RemoveItemKeyValueTags(updateHandle, key.c_str()));
}

JS_METHOD(addItemKeyValueTag) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, key);
	REQ_STR_ARG(2, tagValue);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->AddItemKeyValueTag(updateHandle, key.c_str(), tagValue.c_str()));
}

JS_METHOD(setItemContent) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, contentFolder);

	if (contentFolder.empty()) {
		JS_THROW("contentFolder must be a non-empty string.");
		RET_UNDEFINED;
	}

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemContent(updateHandle, contentFolder.c_str()));
}

JS_METHOD(setItemPreview) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	REQ_STR_ARG(1, previewFile);

	if (previewFile.empty()) {
		JS_THROW("previewFile must be a non-empty string.");
		RET_UNDEFINED;
	}

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->SetItemPreview(updateHandle, previewFile.c_str()));
}

JS_METHOD(submitItemUpdate) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);
	USE_STR_ARG(1, changeNote, "");

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SubmitItemUpdate(updateHandle, changeNote.c_str());
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC submit item update request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackSubmitItemUpdate(env, call));
}

JS_METHOD(getItemUpdateProgress) {
	NAPI_ENV;
	REQ_STR_ARG(0, updateHandleString);

	UGCUpdateHandle_t updateHandle = 0;
	if (!readUgcUpdateHandle(env, updateHandleString, &updateHandle)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint64 bytesProcessed = 0;
	uint64 bytesTotal = 0;
	EItemUpdateStatus status = ugc->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytesTotal);

	Napi::Object result = JS_OBJECT;
	result.Set("status", static_cast<int32_t>(status));
	result.Set("bytesProcessed", jsStringFromUint64(env, bytesProcessed));
	result.Set("bytesTotal", jsStringFromUint64(env, bytesTotal));
	RET_VALUE(result);
}

JS_METHOD(setUserItemVote) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);
	REQ_BOOL_ARG(1, voteUp);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SetUserItemVote(publishedFileId, voteUp);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC set user item vote request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackSetUserItemVote(env, call));
}

JS_METHOD(getUserItemVote) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->GetUserItemVote(publishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC get user item vote request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackGetUserItemVote(env, call));
}

JS_METHOD(addItemToFavorites) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 appId = utils->GetAppID();
	if (info.Length() > 1 && !IS_ARG_EMPTY(1) && !numberToUint32(env, info[1], "appId", &appId)) {
		RET_UNDEFINED;
	}

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->AddItemToFavorites(appId, publishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC add item to favorites request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackFavoriteItemsListChanged(env, call));
}

JS_METHOD(removeItemFromFavorites) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	ISteamUtils *utils = steamUtils(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 appId = utils->GetAppID();
	if (info.Length() > 1 && !IS_ARG_EMPTY(1) && !numberToUint32(env, info[1], "appId", &appId)) {
		RET_UNDEFINED;
	}

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->RemoveItemFromFavorites(appId, publishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC remove item from favorites request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackFavoriteItemsListChanged(env, call));
}

JS_METHOD(addDependency) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);
	REQ_STR_ARG(1, childPublishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	PublishedFileId_t childPublishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId) ||
	    !readPublishedFileId(env, childPublishedFileIdString, &childPublishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->AddDependency(publishedFileId, childPublishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC add dependency request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackAddDependency(env, call));
}

JS_METHOD(removeDependency) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);
	REQ_STR_ARG(1, childPublishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	PublishedFileId_t childPublishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId) ||
	    !readPublishedFileId(env, childPublishedFileIdString, &childPublishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->RemoveDependency(publishedFileId, childPublishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC remove dependency request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackRemoveDependency(env, call));
}

JS_METHOD(subscribeItem) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->SubscribeItem(publishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC subscribe item request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackSubscribeItem(env, call));
}

JS_METHOD(unsubscribeItem) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	SteamAPICall_t call = ugc->UnsubscribeItem(publishedFileId);
	if (call == k_uAPICallInvalid) {
		JS_THROW("Steam UGC unsubscribe item request could not be sent.");
		RET_UNDEFINED;
	}

	RET_VALUE(trackUnsubscribe(env, call));
}

JS_METHOD(getNumSubscribedItems) {
	NAPI_ENV;
	USE_BOOL_ARG(0, includeLocallyDisabled, false);

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_NUM(ugc->GetNumSubscribedItems(includeLocallyDisabled));
}

JS_METHOD(getSubscribedItems) {
	NAPI_ENV;
	USE_UINT32_ARG(0, maxEntries, 0);
	USE_BOOL_ARG(1, includeLocallyDisabled, false);

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint32 subscribedCount = ugc->GetNumSubscribedItems(includeLocallyDisabled);
	uint32 capacity = maxEntries == 0 ? subscribedCount : std::min(maxEntries, subscribedCount);

	std::vector<PublishedFileId_t> publishedFileIds(capacity);
	uint32 count = capacity == 0
	    ? 0
	    : ugc->GetSubscribedItems(publishedFileIds.data(), capacity, includeLocallyDisabled);
	count = std::min(count, capacity);

	Napi::Array result = Napi::Array::New(env, count);
	for (uint32 i = 0; i < count; i++) {
		result.Set(i, jsStringFromUint64(env, publishedFileIds[i]));
	}

	RET_VALUE(result);
}

JS_METHOD(getItemDownloadInfo) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	uint64 bytesDownloaded = 0;
	uint64 bytesTotal = 0;
	bool ok = ugc->GetItemDownloadInfo(publishedFileId, &bytesDownloaded, &bytesTotal);
	if (!ok) {
		RET_NULL;
	}

	Napi::Object result = JS_OBJECT;
	result.Set("bytesDownloaded", jsStringFromUint64(env, bytesDownloaded));
	result.Set("bytesTotal", jsStringFromUint64(env, bytesTotal));
	RET_VALUE(result);
}

JS_METHOD(downloadItem) {
	NAPI_ENV;
	REQ_STR_ARG(0, publishedFileIdString);
	USE_BOOL_ARG(1, highPriority, false);

	PublishedFileId_t publishedFileId = 0;
	if (!readPublishedFileId(env, publishedFileIdString, &publishedFileId)) {
		RET_UNDEFINED;
	}

	ISteamUGC *ugc = steamUgc(env);
	if (env.IsExceptionPending()) {
		RET_UNDEFINED;
	}

	RET_BOOL(ugc->DownloadItem(publishedFileId, highPriority));
}

JS_METHOD(download) {
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
	value.Set("getItemsByIds", Napi::Function::New(env, getItemsByIds));
	value.Set("createItem", Napi::Function::New(env, createItem));
	value.Set("startItemUpdate", Napi::Function::New(env, startItemUpdate));
	value.Set("setItemTitle", Napi::Function::New(env, setItemTitle));
	value.Set("setItemDescription", Napi::Function::New(env, setItemDescription));
	value.Set("setItemMetadata", Napi::Function::New(env, setItemMetadata));
	value.Set("setItemVisibility", Napi::Function::New(env, setItemVisibility));
	value.Set("setItemTags", Napi::Function::New(env, setItemTags));
	value.Set("removeAllItemKeyValueTags", Napi::Function::New(env, removeAllItemKeyValueTags));
	value.Set("removeItemKeyValueTags", Napi::Function::New(env, removeItemKeyValueTags));
	value.Set("addItemKeyValueTag", Napi::Function::New(env, addItemKeyValueTag));
	value.Set("setItemContent", Napi::Function::New(env, setItemContent));
	value.Set("setItemPreview", Napi::Function::New(env, setItemPreview));
	value.Set("submitItemUpdate", Napi::Function::New(env, submitItemUpdate));
	value.Set("getItemUpdateProgress", Napi::Function::New(env, getItemUpdateProgress));
	value.Set("setUserItemVote", Napi::Function::New(env, setUserItemVote));
	value.Set("getUserItemVote", Napi::Function::New(env, getUserItemVote));
	value.Set("addItemToFavorites", Napi::Function::New(env, addItemToFavorites));
	value.Set("removeItemFromFavorites", Napi::Function::New(env, removeItemFromFavorites));
	value.Set("addDependency", Napi::Function::New(env, addDependency));
	value.Set("removeDependency", Napi::Function::New(env, removeDependency));
	value.Set("subscribeItem", Napi::Function::New(env, subscribeItem));
	value.Set("unsubscribeItem", Napi::Function::New(env, unsubscribeItem));
	value.Set("getNumSubscribedItems", Napi::Function::New(env, getNumSubscribedItems));
	value.Set("getSubscribedItems", Napi::Function::New(env, getSubscribedItems));
	value.Set("getItemDownloadInfo", Napi::Function::New(env, getItemDownloadInfo));
	value.Set("downloadItem", Napi::Function::New(env, downloadItem));
	value.Set("download", Napi::Function::New(env, download));
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
