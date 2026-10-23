# GXXE01 reference symbol map

[GXXE01.map](GXXE01.map) preserves the upstream map downloaded on 10 October
2026. The same file was uploaded to this project's `GXXE01.map` resource.

- Source: [StarsMmd/Colo-XD-PBR-symbol-maps](https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps).
- Pinned revision: [`6b51d3afc20ed4c878e2a696ddb22ffaf179981b`](https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps/commit/6b51d3afc20ed4c878e2a696ddb22ffaf179981b).
- Upstream file: [GXXE01.map at that revision](https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps/blob/6b51d3afc20ed4c878e2a696ddb22ffaf179981b/GXXE01.map).
- Upstream commit date: 6 April 2026 UTC.
- SHA-256 of the downloaded and uploaded bytes: `eb3c63654f1aefc3fb6b214e59686d8abf2c4b424a18728ab1e1c0f96b19c532`.
- File size: 2,022,483 bytes.

The upstream commit describes improved symbol matching between games and
regions. This project previously used the map contents from revision
`17ae69ec5c1751f252d70a2cd5477936ab4ffaa6` (31 January 2024).

## Use during reconstruction

The map provides names, retail addresses, and object-path evidence. The
association between a name and a retail address still needs checking against
the executable when it contradicts recovered behaviour. Header filenames,
header splits, and include placement require their own source-quality reasoning.

Examples of corrected associations in this revision:

| Address | Current map entry |
| --- | --- |
| `0x8012566C` | `GXParam1u8`, attributed to `floorFieldCameraEditor.o` |
| `0x800D9040` | `zz_800d9040_`; the project's recovered debugger function name is retained separately |
| `0x80049DC4` | `menuCB_MessageDispOff` |
| `0x801C7D44` | `scriptSetMemoFlag`, attributed to `script.o` |

The build continues to use `config/GXXE01/symbols.txt`. Keeping this reference
file in Git and replacing the uploaded map leaves the existing symbols and
source definitions intact. Changes to recovered names or ownership need a
reviewed reconciliation against the executable and current project source.
