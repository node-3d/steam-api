import { stat, utimes } from 'node:fs/promises';
import { native } from './native.ts';
import { join } from 'node:path';
import type {
	TSteamUgcDetails,
	TSteamNativeUgcNamespace,
	TSteamPublishedFileId,
	TSteamUgcQueryOptions,
	TSteamUgcQueryResult,
	TSteamUgcSaveFilesToCloudResult,
	TSteamWorkshopPublishOptions,
	TSteamWorkshopPublishResult,
	TSteamWorkshopUpdateOptions,
} from './native.ts';

export type {
	TGameId,
	TSteamAccountType,
	TSteamAchievementUnlockTime,
	TSteamAppsNamespace,
	TSteamAuthSessionResponse,
	TSteamAuthSessionTicket,
	TSteamBeginAuthSessionResult,
	TSteamCallbacksNamespace,
	TSteamCallbackEvent,
	TSteamChatEntryType,
	TSteamChatMemberStateChange,
	TSteamCloudFile,
	TSteamCloudNamespace,
	TSteamCloudQuota,
	TSteamCloudSaveFilesResult,
	TSteamDlcData,
	TSteamFloatingGamepadTextInputMode,
	TSteamFriendsNamespace,
	TSteamFriendFlags,
	TSteamFriendGameInfo,
	TSteamFriendMessage,
	TSteamFriendRelationship,
	TSteamId,
	TSteamIdNamespace,
	TSteamImageSize,
	TSteamInitResult,
	TSteamItemPreviewType,
	TSteamItemUpdateStatus,
	TSteamLobbyComparison,
	TSteamLobbyCreated,
	TSteamLobbyChatEntry,
	TSteamLobbyData,
	TSteamLobbyDistanceFilter,
	TSteamLobbyEnter,
	TSteamLobbyId,
	TSteamLobbyMatchList,
	TSteamLobbyType,
	TSteamMatchmakingNamespace,
	TSteamNamespace,
	TSteamNetworkingNamespace,
	TSteamP2PPacket,
	TSteamNativeUgcNamespace,
	TSteamP2PSessionConnectionState,
	TSteamP2PSessionState,
	TSteamP2PSendType,
	TSteamPersonaChange,
	TSteamPublishedFileId,
	TSteamRemoteStoragePublishedFileVisibility,
	TSteamResult,
	TSteamUgcAdditionalPreview,
	TSteamUgcCreateItemResult,
	TSteamUgcDetails,
	TSteamUgcDownloadResult,
	TSteamUgcFavoriteItemsListChangedResult,
	TSteamUgcFileShareResult,
	TSteamUgcGetUserItemVoteResult,
	TSteamUgcHandle,
	TSteamUgcItemDownloadInfo,
	TSteamUgcItemInstallInfo,
	TSteamUgcItemState,
	TSteamUgcItemUpdateProgress,
	TSteamUgcKeyValueTag,
	TSteamUgcMatchingType,
	TSteamUgcPlaytimeStats,
	TSteamUgcQueryKeyValueTag,
	TSteamUgcQueryOptions,
	TSteamUgcQueryResult,
	TSteamUgcQueryType,
	TSteamUgcSaveFilesToCloudResult,
	TSteamUgcSetUserItemVoteResult,
	TSteamUgcSubscribeItemResult,
	TSteamUgcSubmitItemUpdateResult,
	TSteamUgcUnsubscribeResult,
	TSteamUgcUpdateHandle,
	TSteamWorkshopPublishOptions,
	TSteamWorkshopPublishResult,
	TSteamWorkshopUpdateOptions,
	TSteamWorkshopFileType,
	TSteamUserNamespace,
	TSteamUserStatsNamespace,
	TSteamUserUgcList,
	TSteamUserUgcListSortOrder,
	TSteamUtilsNamespace,
} from './native.ts';

export const {
	steam,
	callbacks,
	steamId,
	user,
	utils,
	apps,
	friends,
	matchmaking,
	networking,
	cloud,
	userStats,
	AuthSessionResponse,
	BeginAuthSessionResult,
	FriendFlags,
	FriendRelationship,
	PersonaChange,
	AccountType,
	ChatEntryType,
	ChatMemberStateChange,
	LobbyComparison,
	LobbyDistanceFilter,
	LobbyType,
	Result,
	UGCMatchingType,
	UGCQueryType,
	UserUGCList,
	UserUGCListSortOrder,
	UGCItemState,
	ItemUpdateStatus,
	ItemPreviewType,
	RemoteStoragePublishedFileVisibility,
	WorkshopFileType,
	FloatingGamepadTextInputMode,
	P2PSendType,
	k_ESteamAPIInitResult_OK,
	k_ESteamAPIInitResult_FailedGeneric,
	k_ESteamAPIInitResult_NoSteamClient,
	k_ESteamAPIInitResult_VersionMismatch,
	k_EResultOK,
	k_EResultFail,
	k_EUniverseInvalid,
	k_EUniversePublic,
	k_EUniverseBeta,
	k_EUniverseInternal,
	k_EUniverseDev,
	EncryptedAppTicketSymmetricKeyLength,
} = native;

export type TSteamUgcPublishProgressStage = 'files-saved-to-cloud' | 'files-shared';

export type TSteamUgcPublishProgress = Readonly<{
	stage: TSteamUgcPublishProgressStage;
	files: string[];
}>;

export type TSteamUgcPublishOptions = TSteamWorkshopPublishOptions &
	Readonly<{
		onProgress?: (progress: TSteamUgcPublishProgress) => void;
	}>;

export type TSteamUgcPublishUpdateOptions = TSteamWorkshopUpdateOptions &
	Readonly<{
		onProgress?: (progress: TSteamUgcPublishProgress) => void;
	}>;

export type TSteamUgcSynchronizedItem = TSteamUgcDetails &
	Readonly<{
		isUpdated: boolean;
		path: string;
	}>;

export type TSteamUgcSynchronizeResult = Omit<TSteamUgcQueryResult, 'items'> &
	Readonly<{
		items: TSteamUgcSynchronizedItem[];
	}>;

export type TSteamUgcNamespace = TSteamNativeUgcNamespace &
	Readonly<{
		publish: (
			filePath: string,
			title: string,
			description: string,
			previewFilePath?: string,
			options?: TSteamUgcPublishOptions | null,
		) => Promise<TSteamWorkshopPublishResult>;
		publishUpdate: (
			publishedFileId: TSteamPublishedFileId,
			filePath: string,
			title: string,
			description: string,
			previewFilePath?: string,
			options?: TSteamUgcPublishUpdateOptions | null,
		) => Promise<TSteamWorkshopPublishResult>;
		synchronizeItems: (
			options: TSteamUgcQueryOptions | null | undefined,
			syncDir: string,
		) => Promise<TSteamUgcSynchronizeResult>;
	}>;

const nativeUgc = native.ugc;

const toWorkshopPublishOptions = (
	options: TSteamUgcPublishOptions | null | undefined,
): [TSteamWorkshopPublishOptions, ((progress: TSteamUgcPublishProgress) => void) | undefined] => {
	if (!options) {
		return [{}, undefined];
	}

	return [
		{
			appId: options.appId,
			fileType: options.fileType,
			tags: options.tags,
			visibility: options.visibility,
		},
		options.onProgress,
	];
};

const toWorkshopUpdateOptions = (
	options: TSteamUgcPublishUpdateOptions | null | undefined,
): [TSteamWorkshopUpdateOptions, ((progress: TSteamUgcPublishProgress) => void) | undefined] => {
	if (!options) {
		return [{}, undefined];
	}

	return [
		{
			changeDescription: options.changeDescription,
			tags: options.tags,
			visibility: options.visibility,
		},
		options.onProgress,
	];
};

const stageAndShareFiles = async (
	filePath: string,
	previewFilePath: string,
	onProgress: ((progress: TSteamUgcPublishProgress) => void) | undefined,
): Promise<TSteamUgcSaveFilesToCloudResult> => {
	const cloudFiles = nativeUgc.saveFilesToCloud([filePath, previewFilePath]);
	onProgress?.({ stage: 'files-saved-to-cloud', files: cloudFiles.files });

	await Promise.all(cloudFiles.files.map((file) => nativeUgc.fileShare(file)));
	onProgress?.({ stage: 'files-shared', files: cloudFiles.files });

	return cloudFiles;
};

type TNodeFileError = Readonly<{
	code?: unknown;
}>;

const isMissingFileError = (error: unknown): boolean =>
	typeof error === 'object' &&
	error !== null &&
	'code' in error &&
	(error as TNodeFileError).code === 'ENOENT';

const fileNameFromPath = (value: string): string => value.split(/[\\/]/u).at(-1) ?? value;

const isLocalFileCurrent = async (path: string, timeUpdated: number): Promise<boolean> => {
	try {
		const file = await stat(path);
		return Math.trunc(file.mtimeMs / 1000) === timeUpdated;
	} catch (error) {
		if (isMissingFileError(error)) {
			return false;
		}
		throw error;
	}
};

export const ugc = {
	...nativeUgc,
	publish: async (
		filePath: string,
		title: string,
		description: string,
		previewFilePath = '',
		options?: TSteamUgcPublishOptions | null,
	): Promise<TSteamWorkshopPublishResult> => {
		const [workshopOptions, onProgress] = toWorkshopPublishOptions(options);
		await stageAndShareFiles(filePath, previewFilePath, onProgress);
		return nativeUgc.publishWorkshopFile(
			workshopOptions,
			filePath,
			previewFilePath,
			title,
			description,
		);
	},
	publishUpdate: async (
		publishedFileId: TSteamPublishedFileId,
		filePath: string,
		title: string,
		description: string,
		previewFilePath = '',
		options?: TSteamUgcPublishUpdateOptions | null,
	): Promise<TSteamWorkshopPublishResult> => {
		const [workshopOptions, onProgress] = toWorkshopUpdateOptions(options);
		await stageAndShareFiles(filePath, previewFilePath, onProgress);
		return nativeUgc.updatePublishedWorkshopFile(
			workshopOptions,
			publishedFileId,
			filePath,
			previewFilePath,
			title,
			description,
		);
	},
	synchronizeItems: async (
		options: TSteamUgcQueryOptions | null | undefined,
		syncDir: string,
	): Promise<TSteamUgcSynchronizeResult> => {
		if (!syncDir) {
			throw new Error('syncDir must be a non-empty string.');
		}

		const query = await nativeUgc.getUserItems(
			options,
			UGCMatchingType.Items,
			UserUGCListSortOrder.SubscriptionDateDesc,
			UserUGCList.Subscribed,
		);

		const items = await Promise.all(
			query.items.map(async (item): Promise<TSteamUgcSynchronizedItem> => {
				const fileName = fileNameFromPath(item.fileName);
				if (!fileName) {
					throw new Error('Steam UGC sync item has an empty file name.');
				}

				const path = join(syncDir, fileName);
				if (await isLocalFileCurrent(path, item.timeUpdated)) {
					return { ...item, isUpdated: false, path };
				}

				const download = await nativeUgc.download(item.file, syncDir);
				await utimes(download.path, item.timeUpdated, item.timeUpdated);
				return { ...item, isUpdated: true, path: download.path };
			}),
		);

		return { ...query, items };
	},
} satisfies TSteamUgcNamespace;

export const init = (): void => {
	const result = steam.initEx();

	if (!result.ok) {
		throw new Error(`SteamAPI_InitEx failed with ${result.result}: ${result.errorMessage}`);
	}
};

export const update = (): ReturnType<typeof callbacks.pollCallbacks> => {
	steam.runCallbacks();
	return callbacks.pollCallbacks();
};
