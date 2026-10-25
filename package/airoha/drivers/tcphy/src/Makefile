KERNELDIR := $(KERNEL_DIR)

EXTRA_CFLAGS += -DTCPHY_DEBUG
EXTRA_CFLAGS += -DLINUX_OS

ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
ifeq ($(strip $(TCSUPPORT_ETHPHY)),5R)
EXTRA_CFLAGS+=-DTCSUPPORT_ETHPHY_5R
eth_ephy-objs := tcetherphy_7552.o
else
ifeq ($(strip $(TCSUPPORT_ETHPHY)),0R)
EXTRA_CFLAGS+=-DTCSUPPORT_ETHPHY_0R
eth_ephy-objs := tcetherphy_7552.o
else
EXTRA_CFLAGS+=-DTCSUPPORT_ETHPHY_5R
eth_ephy-objs := tcetherphy_7552.o
endif
endif
else
ifneq ($(strip $(TCSUPPORT_CPU_EN7581)),)
	eth_ephy-objs := tcetherphy_7581.o
else
ifneq ($(strip $(TCSUPPORT_CPU_EN7523)),)
ifeq ($(strip $(ETHPHY)),5R_DIS)
	eth_ephy-objs := tcetherphy_7523_5R_discrete_TXMR.o
else
ifeq ($(strip $(ETHPHY)),5R_TRA)
	eth_ephy-objs := tcetherphy_7523_5R_tradition_TXMR.o
else
ifeq ($(strip $(ETHPHY)),0R_DIS)
	eth_ephy-objs := tcetherphy_7523_0R_discrete_TXMR.o
else
ifeq ($(strip $(ETHPHY)),0R_TRA)
	eth_ephy-objs := tcetherphy_7523_0R_tradition_TXMR.o
else
	eth_ephy-objs := tcetherphy_7523_5R_discrete_TXMR.o
endif
endif
endif
endif
else
ifneq ($(strip $(TCSUPPORT_CPU_EN7580)),)
	eth_ephy-objs := tcetherphy_7580.o
else
ifneq ($(strip $(TCSUPPORT_CPU_EN7528)),)
	eth_ephy-objs := tcetherphy_7528.o
else

ifneq ($(strip $(TCSUPPORT_CPU_EN7516)$(TCSUPPORT_CPU_EN7527)),)
	eth_ephy-objs := tcetherphy_7516.o
else

ifneq ($(strip $(TCSUPPORT_CPU_EN7512)$(TCSUPPORT_CPU_EN7521)),)
eth_ephy-objs := tcetherphy_7512.o
else
	eth_ephy-objs := tcetherphy.o
endif
endif
endif
endif
endif
endif
endif

eth_ephy-objs += tcetherphy_hook.o

obj-m = eth_ephy.o

all: clean
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

