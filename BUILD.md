# Building

The Steamworks SDK is not committed and is not shipped in the npm package.
This follows the Node3D private third-party build input policy recorded in
[ADR 0014](../../docs/adr/0014-private-third-party-build-inputs.md).
Maintainer builds expect an unpacked SDK at:

```text
.sdk/sdk
```

The folder should contain the normal Steamworks layout:

```text
.sdk/sdk/public/steam
.sdk/sdk/redistributable_bin
```

The private archive should be trimmed to those build and runtime inputs.
Steamworks SDK folders such as `tools` and `steamworksexample` are not needed
for addon builds.

To hydrate that folder in CI, upload a private `steamworks-sdk.gz` archive and
set:

```text
STEAM_SDK_GZ_LINK
STEAM_SDK_GZ_SHA
```

`STEAM_SDK_GZ_LINK` may point to any URL that returns the archive bytes without
an interactive browser page. `STEAM_SDK_GZ_SHA` is required for downloaded
archives.

For local maintainer builds, place the archive at:

```text
.sdk/steamworks-sdk.gz
```

`npm run sdk:ensure` unpacks that archive into `.sdk/sdk`.
