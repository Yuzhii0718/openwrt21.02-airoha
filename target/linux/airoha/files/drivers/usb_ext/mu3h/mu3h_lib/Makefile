#
# Makefile for USB Host Lib
#

ifeq ($(CONFIG_USB_MU3H_DRV),y)
    EXTRA_CFLAGS += -I$(srctree)/drivers/usb_ext/mu3h/mu3h_drv 
#    EXTRA_CFLAGS += -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3h/mu3h_drv 
#    EXTRA_CFLAGS += -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3h/mu3h_drv 
endif

ifeq ($(CONFIG_USB_MU3H_TEST_DRV),y)
#    EXTRA_CFLAGS += -Idrivers/usb_ext/mu3h/mu3h_test_drv 
    EXTRA_CFLAGS += -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3h/mu3h_test_drv 
endif

#EXTRA_CFLAGS += -Idrivers/usb_ext/mu3_phy
#EXTRA_CFLAGS += -I$(srctree)/../$(KERNEL_USB_EXT_DIR)/drivers/usb_ext/mu3_phy
EXTRA_CFLAGS += -I$(srctree)/drivers/usb_ext/mu3h/mu3h_drv 
EXTRA_CFLAGS += -I$(srctree)/drivers/usb_ext/mu3_phy
EXTRA_CFLAGS += -I../../mu3_phy
obj-$(CONFIG_USB_MU3H)	+= xhci-mtk.o xhci-mtk-power.o xhci-mtk-scheduler.o
#obj-$(CONFIG_USB_MU3H)	+= xhci-mtk.o xhci-mtk-power.o xhci-mtk-sch.o
