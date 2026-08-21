import { install } from '@node-3d/addon-tools';

const prefix = 'https://github.com/node-3d/steam-api/releases/download';
const tag = '0.3.0';

await install(`${prefix}/${tag}`);
