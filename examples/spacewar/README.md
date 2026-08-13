# Spacewar

Spacewar is Valve's Steamworks API example application. Valve documents its
example AppID as `480`.

When running outside Steam, Steamworks needs to know the app id before
initialization. Valve documents `steam_appid.txt` as a development-only file
containing only the AppID, and says Steam looks for it in the current working
directory.

Run the playable demo from its folder so Steam can find the checked-in
`steam_appid.txt`:

```powershell
cd examples/spacewar
node main.ts
```

`main.ts` checks that `steam_appid.txt` exists in the current working directory
before initializing Steam. It opens a Node3D window and uses Spacewar AppID 480
for Steam callbacks, rich presence, and overlay entry points.

Controls are printed to the terminal when the game starts:

- `W` / `ArrowUp` - thrust.
- `A` / `ArrowLeft` - turn left.
- `D` / `ArrowRight` - turn right.
- `Space` - fire.
- `Shift` - shield.
- `R` - restart.
- `F1` - Steam Friends overlay.
- `F2` - Steam Achievements overlay.
- `F3` - Steam Community overlay.
- `F4` - Steam Store overlay.
- `F5` - Steam Community web page overlay for AppID 480.

The old terminal-only Steam snapshot lives in `stats.ts`:

```powershell
cd examples/spacewar
node stats.ts
```
