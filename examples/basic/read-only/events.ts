import { initSteam, logEvent, numberOption, pumpFor } from '../helpers.ts';

initSteam();

const seconds = numberOption('seconds', 30);
console.log(`Pumping Steam callbacks for ${seconds} seconds.`);

await pumpFor(seconds * 1000, logEvent);
