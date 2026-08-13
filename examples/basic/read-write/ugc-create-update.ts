import { existsSync } from 'node:fs';
import {
	ItemUpdateStatus,
	RemoteStoragePublishedFileVisibility,
	WorkshopFileType,
	ugc,
	utils,
} from '@node-3d/steam-api';
import {
	hasFlag,
	initSteam,
	listOption,
	option,
	publishedFileIdFromOption,
	requiredOption,
	waitForSteamCall,
} from '../helpers.ts';

initSteam();

if (!hasFlag('yes-create-workshop-item')) {
	throw new Error(
		'This example creates or updates Steam Workshop content. Pass --yes-create-workshop-item to continue.',
	);
}

const appId = Number(option('app-id') ?? utils.getAppId());
const content = requiredOption('content');
const preview = requiredOption('preview');
const title = option('title') ?? 'Node3D Steam API Workshop Item';
const description = option('description') ?? 'Created by @node-3d/steam-api example.';
const changeNote = option('change-note') ?? 'Updated by @node-3d/steam-api example.';
const tags = listOption('tags');

if (!existsSync(content)) {
	throw new Error(`Content path does not exist: ${content}`);
}

if (!existsSync(preview)) {
	throw new Error(`Preview path does not exist: ${preview}`);
}

const created = option('published-file-id')
	? null
	: await waitForSteamCall(ugc.createItem(appId, WorkshopFileType.Community));
const publishedFileId = created?.publishedFileId ?? publishedFileIdFromOption('published-file-id');

console.log('Workshop item:', publishedFileId);

const updateHandle = ugc.startItemUpdate(appId, publishedFileId);
console.log('Set title:', ugc.setItemTitle(updateHandle, title));
console.log('Set description:', ugc.setItemDescription(updateHandle, description));
console.log(
	'Set metadata:',
	ugc.setItemMetadata(updateHandle, JSON.stringify({ source: 'node3d' })),
);
console.log(
	'Set visibility:',
	ugc.setItemVisibility(updateHandle, RemoteStoragePublishedFileVisibility.Private),
);
console.log('Set tags:', ugc.setItemTags(updateHandle, tags));
console.log('Set content:', ugc.setItemContent(updateHandle, content));
console.log('Set preview:', ugc.setItemPreview(updateHandle, preview));

let lastProgressLog = 0;
const submitted = await waitForSteamCall(ugc.submitItemUpdate(updateHandle, changeNote), {
	timeoutMs: 300_000,
	onTick: () => {
		const now = Date.now();
		if (now - lastProgressLog < 1000) {
			return;
		}

		lastProgressLog = now;
		const progress = ugc.getItemUpdateProgress(updateHandle);
		if (progress.status !== ItemUpdateStatus.Invalid) {
			console.log('Submit progress:', progress);
		}
	},
});

console.log('Submitted:', submitted);
