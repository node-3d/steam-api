# Building

The Steamworks SDK is not committed and is not shipped in the npm package.
Maintainer builds expect an unpacked SDK at:

```text
.sdk/sdk
```

The folder should contain the normal Steamworks layout:

```text
.sdk/sdk/public/steam
.sdk/sdk/redistributable_bin
```

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
