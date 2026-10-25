# feeds-patches/ - product delta on top of the upstream feeds

`feeds/routing`, `feeds/packages` and `feeds/luci` are **not tracked in git**.
They are fetched by `scripts/feeds update` at the commit pinned in
`feeds.conf.default`, and everything the product changed on top of that is stored
here as patch files. `feeds/airoha` and `feeds/mtk_openwrt_feed` have no public
remote at all and stay tracked.

`luci-theme-argon` is not part of this delta any more: it was moved to
`package/luci-theme-argon/` so that 1.1 MB of theme files (24 of them binary) do
not have to live inside a patch.

## Apply

```sh
scripts/feeds-vendor-patch.sh
```

The script fetches a managed feed when it is missing, applies every patch under
`feeds-patches/<feed>/`, and skips the ones that are already applied, so it is
safe to run on every build. `autobuild/airoha/lede-build-sanity.sh` calls it
before `scripts/feeds update -i` (a fresh checkout has no feed directory to index
until the script has fetched it).

The patches are `git diff --binary` output (avahi and nginx carry patch files,
miniupnpd ships whole sources), so they must be applied with `git apply`, not
`patch(1)`.

## Content

| patch | what it carries |
|---|---|
| `routing/001-nodogsplash.patch` | `files/` (nodogsplash.conf, init script), `patches/`, Makefile hook |
| `packages/001-libs-avahi.patch` | the mywifiext mDNS work: `files/` helpers (mdnsrecord, updateservice, ...), 9 `0xx-*` patches, avahi-daemon.conf/init changes, Makefile |
| `packages/002-libs-expat.patch` | Makefile (package split/host build) |
| `packages/003-libs-redis.patch` | Makefile, `020-fix-atomicvar` refresh |
| `packages/004-net-nginx.patch` | `Config_ssl.in`, Makefile, luci-support snippet, `201-ignore-invalid-options` |
| `packages/005-net-samba4.patch` | `files/samba.init`, `files/smb.conf.template` |
| `packages/006-net-miniupnpd.patch` | WSC support: Makefile, `files/`, `patch_tmp_*`, `300..304` patches (includes the vendor's `.orig`/`.rej` leftovers) |
| `luci/001-modules-luci-base.patch` | `luci-base/Makefile`, `luci-base/root/etc/config/luci` |
| `luci/002-collections.patch` | `collections/luci`, `collections/luci-light` |
| `luci/003-modules-luci-mod-system.patch` | `luci-mod-system/Makefile` |

Excluded on purpose, they are packaging artefacts rather than product changes:

* `.github/`, `.gitattributes`, `.gitignore` - the GPL drop does not ship them,
  and `.gitattributes` would normalise line endings and break the round trip;
* `luci-base/src/{jsmin,po2lmo,*.o,plural_formula.c,plural_formula.h,contrib/lemon}`
  - host tools and objects rebuilt by luci-base's own Makefile;
* file modes - the vendor copy marks everything 0755;
* `luci/applications/luci-app-olsr/htdocs/cgi-bin-nodes.html` - upstream ships it
  as a symlink to `cgi-bin-status.html`, the vendor copy dereferenced it. Same
  content, so the symlink coming back from upstream is fine.

## Regenerate after a feed update

When `feeds.conf.default` is bumped to a new commit:

```sh
# 1. fetch the new upstream tree
git init /tmp/base && cd /tmp/base
git remote add o https://github.com/openwrt/packages      # or ./luci, ./routing
git fetch --depth 1 o <new-sha> && git checkout FETCH_HEAD

# 2. build the baseline repo, copy the product tree over it, ignoring the
#    artefacts listed above
rm -rf .github .gitattributes .gitignore
git -c core.fileMode=false init -q . && git add -A && git commit -qm base
rsync -a --delete --exclude .git --exclude .github --exclude .gitattributes \
      --exclude .gitignore --exclude 'jsmin*' --exclude 'po2lmo*' \
      --exclude 'plural_formula.*' --exclude '*.o' --exclude 'contrib/lemon' \
      <topdir>/feeds/<feed>/ ./
git -c core.fileMode=false add -A

# 3. export one patch per package (see the table above for the split)
git -c core.autocrlf=false diff --binary --cached HEAD -- libs/avahi \
    > <topdir>/feeds-patches/packages/001-libs-avahi.patch
```

Acceptance test: apply the patch set to a clean upstream tree and check

```sh
diff -rq -x .github -x .gitattributes -x .gitignore -x 'jsmin*' -x 'po2lmo*' \
     -x 'plural_formula.*' -x '*.o' -x lemon -x cgi-bin-nodes.html \
     <patched> feeds/<feed>
```

prints nothing for every managed feed - that is what the current patch set does.
