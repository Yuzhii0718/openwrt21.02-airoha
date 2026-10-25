#
# Makefile for USB Host Controller Drivers
#

EXTRA_CFLAGS += -O0

ifeq ($(CONFIG_USB_DEBUG),y)
	EXTRA_CFLAGS		+= -DDEBUG
endif
#EXTRA_CFLAGS += -Idrivers/usb/mu3h/mu3h_lib \
#                -Idrivers/usb/mu3_phy
#EXTRA_CFLAGS += -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3h/mu3h_lib \
#                -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3_phy
EXTRA_CFLAGS += -I$(srctree)/drivers/usb_ext/mu3h/mu3h_lib \
                -I$(srctree)/drivers/usb_ext/mu3_phy


obj-$(CONFIG_USB_MU3H_DRV)	+= xhci.o xhci-mem.o xhci-plat.o xhci-ring.o xhci-hub.o xhci-dbg.o xhci-trace.o
#obj-$(CONFIG_USB_XHCI_HCD)	+= xhci.o xhci-mem.o xhci-plat.o xhci-ring.o xhci-hub.o xhci-dbg.o xhci-trace.o

