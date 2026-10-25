#
# Makefile for MTK USB3.0 PHY
#

ifeq ($(CONFIG_USB_DEBUG),y)
	EXTRA_CFLAGS		+= -DDEBUG
endif

#obj-$(CONFIG_USB_MU3H)	+= mtk-phy.o mtk-phy-c60802.o mtk-phy-d60802.o mtk-phy-e60802.o mtk-phy-a60810.o mtk-phy-gpio.o mtk-phy-ahb.o #mtk-phy-5399.o #mtk-phy-7662.0
ifneq ($(strip $(TCSUPPORT_CPU_EN7581)),)
	obj-y += mtk-phy_hook.o
	obj-y += mtk-phy_7581.o
else 
ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
	obj-y += mtk-phy_hook.o
	obj-y += mtk-phy_7552.o
else
	obj-y += mtk-phy.o
endif #else
endif #ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)

