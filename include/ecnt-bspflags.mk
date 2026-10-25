#
# Reproduce the BSP_CFLAGS that the target build derives in
# target/linux/airoha/rule.mak, so that vendor driver packages can be built
# as external kernel modules.
#
# The vendor build compiles these drivers inside the kernel build tree, where
# rule.mak has already exported BSP_CFLAGS (a long list of -DTCSUPPORT_* plus
# include paths) and the kernel Makefile folds it into KBUILD_CFLAGS. Package
# builds never see that variable, which leaves the chip/feature #ifdefs unset
# and silently selects the wrong code paths.
#
# Usage, from a package Makefile, after rules.mk/kernel.mk:
#
#	include $(INCLUDE_DIR)/ecnt-bspflags.mk
#	...
#	$(MAKE) -C "$(LINUX_DIR)" ... BSP_CFLAGS="$(BSP_CFLAGS)" modules
#
# This file only assigns variables; it defines no targets.
#

# rule.mak unconditionally includes tclinux-only fragments (Project/*.mak)
# when OPENWRT_VERBOSE is empty; pretend to be verbose while parsing it.
_ECNT_VERBOSE_SAVED := $(OPENWRT_VERBOSE)
ifeq ($(strip $(OPENWRT_VERBOSE)),)
  OPENWRT_VERBOSE := 1
  _ECNT_VERBOSE_FORCED := 1
endif

-include $(wildcard $(TOPDIR)/target/linux/airoha/$(SUBTARGET)/*.mak)
include $(TOPDIR)/target/linux/airoha/dir.mak
include $(TOPDIR)/target/linux/airoha/rule.mak

ifeq ($(_ECNT_VERBOSE_FORCED),1)
  OPENWRT_VERBOSE := $(_ECNT_VERBOSE_SAVED)
  undefine _ECNT_VERBOSE_FORCED
endif
undefine _ECNT_VERBOSE_SAVED

# The Kbuild makefiles of the vendor modules test these switches with ifneq().
export $(filter TCSUPPORT_%,$(.VARIABLES))
