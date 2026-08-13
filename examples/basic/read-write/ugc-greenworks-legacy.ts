import { RemoteStoragePublishedFileVisibility, ugc } from '@node-3d/steam-api';
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

const files = listOption('files');
if (files.length > 0) {
	const saved = ugc.saveFilesToCloud(files);
	console.log('Saved files to cloud:', saved);

	if (hasFlag('share')) {
		const shares = await Promise.all(
			saved.files.map((file) => waitForSteamCall(ugc.fileShare(file))),
		);
		console.log('Shared files:', shares);
	}
}

if (hasFlag('publish')) {
	const file = requiredOption('file');
	const preview = requiredOption('preview');
	const title = option('title') ?? 'Node3D legacy workshop publish';
	const description = option('description') ?? 'Published through Greenworks-compatible helpers.';
	const publish = ugc.publish(file, title, description, preview, {
		tags: listOption('tags'),
		visibility: RemoteStoragePublishedFileVisibility.Private,
		onProgress: (progress) => console.log('Publish progress:', progress),
	});

	console.log('Publish:', await waitForSteamCall(publish));
}

if (hasFlag('update')) {
	const publishedFileId = publishedFileIdFromOption('published-file-id');
	const file = requiredOption('file');
	const preview = requiredOption('preview');
	const title = option('title') ?? 'Node3D legacy workshop update';
	const description = option('description') ?? 'Updated through Greenworks-compatible helpers.';
	const update = ugc.publishUpdate(publishedFileId, file, title, description, preview, {
		changeDescription: option('change-description') ?? 'Node3D example update',
		tags: listOption('tags'),
		visibility: RemoteStoragePublishedFileVisibility.Private,
		onProgress: (progress) => console.log('Update progress:', progress),
	});

	console.log('Update:', await waitForSteamCall(update));
}

const syncDir = option('sync-dir');
if (syncDir) {
	console.log(
		'Synchronize items:',
		await waitForSteamCall(ugc.synchronizeItems({ page: 1 }, syncDir)),
	);
}
