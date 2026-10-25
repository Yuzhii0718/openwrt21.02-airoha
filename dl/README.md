# dl/ - downloaded sources

`dl/` holds every package tarball the build consumes. The directory is
gitignored, with one exception: blobs that have **no public download location**
are force-added (`git add -f`), because `make download` cannot recreate them.
Everything else is ordinary third-party source that the URLs recorded in the
package Makefiles (`PKG_SOURCE_URL`, `PKG_SOURCE_PROTO`, `PKG_HASH`) reproduce.

## Tracked: vendor drops (`PKG_SOURCE_URL` is empty, cannot be fetched)

| file | used by |
|---|---|
| `mt799x_20250613-415824.tar.xz` | `package/mtk/drivers/mt_wifi7`, `package/mtk/drivers/mt_hwifi` |
| `mt799x_20250613-415824-bin.tar.xz` | the same two packages (wifi firmware, includes the BE3600 EEPROMs) |
| `mt_wifi_osal-a53418.tar.xz` | `package/mtk/drivers/mt_wifi_osal` |
| `mapd-5f8735.tar.bz2` | `package/mtk/applications/mapd` |
| `wappd-1d4946.tar.bz2` | `package/mtk/applications/wappd` |
| `1905daemon-d3aa20.tar.bz2` | `package/mtk/applications/1905daemon` |
| `libmapd-007ce6.tar.bz2` | `package/mtk/applications/libmapd` |
| `libwpactrl-f4016d.tar.bz2` | `package/mtk/applications/libwpactrl` |
| `datconf-607478.tar.bz2` | `package/mtk/applications/datconf` |
| `fwdd-ed4401.tar.bz2` | `package/mtk/applications/fwdd` |
| `sigma_daemon-Kite8212.tar.bz2` | `package/mtk/applications/sigma_daemon` |
| `sigma_dut-Kite8212.tar.bz2` | `package/mtk/applications/sigma_dut_v9.0.0` |
| `mapfilter-1a8f03.tar.bz2` | `package/mtk/drivers/mapfilter` |
| `mtfwd-7a74da.tar.bz2` | `package/mtk/drivers/mtfwd` |
| `libnvram-9d068860.tar.gz` | `package/tw/applications/libnvram` |
| `uhttpd_server-2020-5-13-5e9c23c6.tar.xz` | `package/tw/applications/uhttpd_server` |

`package/mtk/drivers/warp` is built from `./src` inside the package and does not
consume a tarball, even though the vendor publishes `warp_driver/` inside the
wifi drop.

## Tracked: pinned mirror tarballs (`PKG_SOURCE_URL` is empty)

| file | used by |
|---|---|
| `libnl-tiny-2023-12-05-965c4bf4.tar.xz` | `package/libs/mtk_libnl-tiny` |
| `libiwinfo-2024-07-06-21582013.tar.xz` | `package/network/utils/mtk_iwinfo` |

Both are source-ized replacements of vendor blobs that pin the exact upstream
mirror tarball (the `PKG_HASH` is the upstream mirror hash) while leaving the URL
empty, so they have to come from git as well. Without them the build fails in
`Build/Prepare` with `xzcat: ...: No such file or directory`, not in
`make download`.

## Not tracked

Everything else in `dl/` (toolchain, kernel, busybox, backports, host tools, ...
about 650 MB) is not tracked. A fresh clone starts with only the files above; run

```
make download
```

once on a machine with network access to refill the rest. The build verifies each
file against its `PKG_HASH`, so a stale or patched copy is rejected.

The retired 2024-09 Kite wifi drop (`mt7992-Kite8212.tar.xz`, tracked here until
the 2025-06-13 migration) now lives in `archive/mtwifi/`; nothing references it
any more.

The list can always be checked with `git ls-files dl/`.
