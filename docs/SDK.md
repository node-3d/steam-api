# Steamworks SDK

The Steamworks SDK is a private third-party build input. Do not commit the SDK,
the archive, or extracted `.sdk/` contents.

## Create Or Update The Archive

Download the SDK from Valve:

```text
https://partner.steamgames.com/downloads/steamworks_sdk.zip
```

Unpack the downloaded zip and use the top-level `sdk` folder from it.

Optionally trim the unpacked SDK to only the inputs used by this package:

```text
sdk/public/
sdk/redistributable_bin/
sdk/Readme.txt
```

This usually reduces the private archive size by about 10x. The addon does not
need Steamworks folders such as `tools` or `steamworksexample` for builds.

From the directory that contains the unpacked `sdk` folder, create the archive:

```bash
tar -czf steamworks-sdk.gz -C sdk .
```

The archive should contain the contents of `sdk`, not an outer `sdk` directory.

## Local Maintainer Setup

Place or replace the archive at `.sdk/steamworks-sdk.gz`.
Then hydrate the local SDK folder:

```bash
rm -rf .sdk/sdk
npm run sdk:ensure
```

The resulting layout must include:

```text
.sdk/sdk/public/
.sdk/sdk/redistributable_bin/
```

## CI Setup

Hash the new archive:

```bash
sha256sum steamworks-sdk.gz | awk '{print $1}'
```

On macOS:

```bash
shasum -a 256 steamworks-sdk.gz | awk '{print $1}'
```

On PowerShell:

```powershell
(Get-FileHash .\steamworks-sdk.gz -Algorithm SHA256).Hash.ToLower()
```

Upload `steamworks-sdk.gz` to private storage, such as Google Drive. The URL
must return the archive bytes directly, not an interactive browser page.

Set these repository-level GitHub Actions secrets on the standalone
`steam-api` repository:

```text
STEAM_SDK_GZ_LINK
STEAM_SDK_GZ_SHA
```

`STEAM_SDK_GZ_LINK` points to the private archive URL. `STEAM_SDK_GZ_SHA` is the
SHA-256 hash of that exact archive. CI uses the hash both to verify downloads
and to invalidate the cached `.sdk` folder when the SDK changes.
