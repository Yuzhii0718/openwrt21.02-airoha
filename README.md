> [!CAUTION]
> Contains software code developed in part by third parties, such as Airoha/EcoNet/Mediatek/NETGEAR/TP-Link, including software code subject to the GNU General Public License ("GPL") or GNU Lesser General Public License ("LGPL"). Please see the [GPL](https://www.gnu.org/licenses/gpl.txt) and [LGPL](https://www.gnu.org/licenses/lgpl.txt) Websites to view the terms of each license.

**The code for this project is based on GPL open-source code provided by Netgear and TP-Link.**

![OpenWrt logo](include/logo.png)

OpenWrt Project is a Linux operating system targeting embedded devices. Instead
of trying to create a single, static firmware, OpenWrt provides a fully
writable filesystem with package management. This frees you from the
application selection and configuration provided by the vendor and allows you
to customize the device through the use of packages to suit any application.
For developers, OpenWrt is the framework to build an application without having
to build a complete firmware around it; for users this means the ability for
full customization, to use the device in ways never envisioned.

Sunshine!

## Development

To build your own firmware you need a GNU/Linux, BSD or MacOSX system (case
sensitive filesystem required). Cygwin is unsupported because of the lack of a
case sensitive file system.

### Requirements

You need the following tools to compile OpenWrt, the package names vary between
distributions. A complete list with distribution specific packages is found in
the [Build System Setup](https://openwrt.org/docs/guide-developer/build-system/install-buildsystem)
documentation.

```
gcc binutils bzip2 flex python3 perl make find grep diff unzip gawk getopt
subversion libz-dev libc-dev rsync which
```

### AN7563 Quickstart

- Prepare
  * **Clone the repository**:

    ```bash
    git clone --branch main <repository_url>
    ```

    > Another branch contains a full history of original commits and is exceptionally large; unless necessary, clone only the main branch to save resources.

  * **Public feeds**:

    ```bash
    ./scripts/feeds update -a
    ./scripts/feeds-vendor-patch.sh
    ./scripts/feeds install -a
    ```

  * **Third-party source tarballs**:

    ```bash
    cp autobuild/airoha/an7563-mt7992-EXS27/.config .config
    make download
    ```

- Auto build

Run `bash autobuild/airoha/an7563-mt7992-EXS27/lede-branch-build-sanity.sh` to build firmwares at first. Then you can add `build` after the script to build again.

- Manual build

get a `.config` from autobuild directory, and run `make defconfig` to generate `.config`. Then run `make MSDK=1 V=s -j$(nproc) PKG_HASH=skip PKG_MIRROR_HASH=skip` to build.

### Related Repositories

The main repository uses multiple sub-repositories to manage packages of
different categories. All packages are installed via the OpenWrt package
manager called `opkg`. If you're looking to develop the web interface or port
packages to OpenWrt, please find the fitting repository below.

* [LuCI Web Interface](https://github.com/openwrt/luci): Modern and modular
  interface to control the device via a web browser.

* [OpenWrt Packages](https://github.com/openwrt/packages): Community repository
  of ported packages.

* [OpenWrt Routing](https://github.com/openwrt/routing): Packages specifically
  focused on (mesh) routing.

## Support Information

For a list of supported devices see the [OpenWrt Hardware Database](https://openwrt.org/supported_devices)

### Documentation

* [Quick Start Guide](https://openwrt.org/docs/guide-quick-start/start)
* [User Guide](https://openwrt.org/docs/guide-user/start)
* [Developer Documentation](https://openwrt.org/docs/guide-developer/start)
* [Technical Reference](https://openwrt.org/docs/techref/start)

### Support Community

* [Forum](https://forum.openwrt.org): For usage, projects, discussions and hardware advise.
* [Support Chat](https://webchat.oftc.net/#openwrt): Channel `#openwrt` on **oftc.net**.

### Developer Community

* [Bug Reports](https://bugs.openwrt.org): Report bugs in OpenWrt
* [Dev Mailing List](https://lists.openwrt.org/mailman/listinfo/openwrt-devel): Send patches
* [Dev Chat](https://webchat.oftc.net/#openwrt-devel): Channel `#openwrt-devel` on **oftc.net**.

## License

OpenWrt is licensed under GPL-2.0
