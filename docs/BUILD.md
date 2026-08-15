# Building

The Steamworks SDK is not committed and is not shipped in the npm package.
This follows the Node3D private third-party build input policy recorded in
[ADR 0014](../../../docs/adr/0014-private-third-party-build-inputs.md).
See [SDK.md](SDK.md) for the maintainer steps to create or update the private
`steamworks-sdk.gz` archive.

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

CI caches `.sdk` by `STEAM_SDK_GZ_SHA`. Updating the private Steamworks SDK
archive requires updating the hash secret so GitHub Actions invalidates the old
cache and rehydrates the SDK from `STEAM_SDK_GZ_LINK`.

The native release build matrix follows the standard Node3D addon targets
supported by the Steamworks SDK inputs: Windows x64, Linux x64, Linux ARM64,
macOS x64, and macOS ARM64. Windows ARM64 is intentionally omitted until the SDK
provides matching Windows ARM64 import and runtime libraries.

The test workflow also runs a `windows-x64-on-arm` lane on GitHub's
`windows-11-arm` runner with x64 Node.js. That mirrors the expected Windows ARM
Steam usage path: Steam and the addon run as x64 Windows binaries under Windows
on ARM emulation. This lane validates the normal Windows x64 package artifact on
an ARM host; it does not produce a separate Windows ARM64 release binary.
