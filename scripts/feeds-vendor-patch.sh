#!/bin/sh
#
# Apply the product delta on top of the upstream OpenWrt feeds.
#
# feeds/routing and feeds/packages are not tracked in git: they are fetched by
# "scripts/feeds update" at the commit pinned in feeds.conf.default, and the
# product changes to them live in feeds-patches/<feed>/*.patch. Those patches are
# plain "git diff --binary" output (avahi/nginx pull in patch files, and the luci
# theme brings images), so they must be applied with git apply, not patch(1).
#
# Run it after "scripts/feeds update" / before "scripts/feeds install".
# It is idempotent: an already applied patch is detected and skipped.

set -e

TOPDIR=$(cd "$(dirname "$0")/.." && pwd)
cd "$TOPDIR"

# Feeds whose product delta lives in feeds-patches/
MANAGED_FEEDS="routing packages luci"

for feed in $MANAGED_FEEDS; do
	feeddir="feeds/$feed"

	if [ ! -d "$feeddir" ]; then
		echo "feeds/$feed missing, fetching it (commit pinned in feeds.conf.default)"
		./scripts/feeds update "$feed"
	fi

	for patch in "feeds-patches/$feed"/*.patch; do
		[ -e "$patch" ] || continue
		name=$(basename "$patch")

		# already applied?
		if git apply --check --reverse -p1 --directory="$feeddir" "$patch" 2>/dev/null; then
			echo "  skip   $feed/$name (already applied)"
			continue
		fi

		if ! git apply --check -p1 --directory="$feeddir" "$patch" 2>/dev/null; then
			echo "  ERROR  $feed/$name does not apply - does feeds/$feed match" \
			     "the commit pinned in feeds.conf.default?" >&2
			exit 1
		fi

		git apply -p1 --directory="$feeddir" "$patch"
		echo "  apply  $feed/$name"
	done
done

echo "vendor feed patches are in place"
