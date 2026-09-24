import { install } from '@node-3d/addon-tools';

const prefix = 'https://github.com/node-3d/steam-api/releases/download';
const tag = '0.4.1';
// oxlint-disable-next-line node/no-process-env
const candidateUrl = process.env.NODE_3D_STEAM_API_BINARY_BASE_URL;

await install(candidateUrl || `${prefix}/${tag}`);
