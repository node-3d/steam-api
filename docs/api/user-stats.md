# Stats and achievements

Official reference: [ISteamUserStats](https://partner.steamgames.com/doc/api/ISteamUserStats).

The synchronous getters use `null` for a missing/unavailable stat or
achievement. Use `getStatInt(name)` or `getStatFloat(name)` when the schema type
is known; `setStat(name, value)` dispatches by numeric value, while
`setStatInt()` and `setStatFloat()` make that choice explicit.

```ts
const deaths = userStats.getStatInt('deaths') ?? 0;
userStats.setStatInt('deaths', deaths + 1);
userStats.setAchievement('first_win');

if (!userStats.storeStats()) {
	throw new Error('Steam did not accept the stats store request');
}
```

`getAchievement(name)` returns `boolean | null`; use `setAchievement()`,
`clearAchievement()`, and
`indicateAchievementProgress(name, currentProgress, maxProgress)` to change
state. `storeStats()` requests persistence. Observe `user-stats-stored` and
`user-achievement-stored` events to handle later Steam responses.

For schema inspection, use `getNumAchievements()`, `getAchievementNames()`,
`getAchievementName(index)`, `getAchievementDisplayAttribute(name, key)`, and
`getAchievementAndUnlockTime(name)`. Indexed lookup methods may return `null`.
`resetAllStats(achievementsToo)` is destructive and should be restricted to
development/debug tooling.

`getNumberOfCurrentPlayers()` is a Steam call-result promise, so callback
pumping is required. The initial user-stats readiness event is exposed as
`user-stats-received`; wait for a successful result before depending on stored
stats or achievements after startup.

