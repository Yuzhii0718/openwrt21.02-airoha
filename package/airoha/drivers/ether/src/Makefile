ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

KERNELDIR := $(KERNEL_DIR)

EXTRA_CFLAGS +=-DTCPHY_SUPPORT -DRT63365_SUPPORT -DMII_INTERFACE -DWAN2LAN \
#							-DCONFIG_SUPPORT_SELF_TEST

EXTRA_CFLAGS += -DLINUX_OS 

ifneq ($(TCSUPPORT_AUTOBENCH),)
EXTRA_CFLAGS += -DTCPHY_DEBUG
endif
#EXTRA_CFLAGS +=-DLOOPBACK_SUPPORT
#EXTRA_CFLAGS+=-DLOOPBACK_SUPPORT_JUMBO

eth-objs := eth_lan.o eth_proc.o eth_api.o eth_led.o eth_ext_switch.o ethtool_support.o

ifneq ($(TCSUPPORT_ECNT_1588v2),)
eth-objs += eth_ptp.o
endif

ifeq ($(strip $(TCSUPPORT_CPU_EN7516) $(TCSUPPORT_CPU_EN7527) $(TCSUPPORT_CPU_EN7580)),)
ifneq ($(TCSUPPORT_AUTOBENCH),)
eth-objs += ../../auto_bench/femac_autobench.o
endif
endif

eth-objs +=./tcphy/tcswitch.o
eth-objs +=./tcphy/tcconsole.o
eth-objs +=./tcphy/phy_api.o
eth-objs +=./tcphy/switch_proc.o
#eth-objs += fe_verify.o

ifneq ($(strip $(TCSUPPORT_SWITCH_8850)),)
eth-objs += ../en8850/en8850_init.o ../en8850/en8850_api.o
eth-objs += ../en8850/en8850_mdio.o ../en8850/en8850_phy_cal.o ../en8850/en8850_phy_cal_dump.o 
eth-objs += ../en8850/ecnt_aml.o ../en8850/ecnt_diag.o ../en8850/ecnt_error.o
eth-objs += ../en8850/ecnt_init.o ../en8850/ecnt_l2.o ../en8850/ecnt_lag.o ../en8850/ecnt_led.o 
eth-objs += ../en8850/ecnt_mib.o ../en8850/ecnt_mirror.o ../en8850/ecnt_port.o ../en8850/ecnt_qos.o 
eth-objs += ../en8850/ecnt_sec.o ../en8850/ecnt_stp.o ../en8850/ecnt_switch.o ../en8850/ecnt_vlan.o
eth-objs += ../en8850/ecnt_acl.o ../en8850/ecnt_sptag.o
#eth-objs += ../en8850/ecnt_cmd.o 
endif

ifneq ($(strip $(TCSUPPORT_SWITCH_8851)),)
eth-objs += ../en8851/en8851_init.o 
endif

ifneq ($(strip $(TCSUPPORT_MT7530_SWITCH_API)),)
eth-objs += ./tcphy/mtkswitch_api_krl.o
endif
ifneq ($(TCSUPPORT_HEC_6906),)
eth-objs += access_module_cl.o
endif
ifneq ($(TCSUPPORT_WAN_ETHER),)
eth-objs += eth_wan.o
endif
ifneq ($(TCSUPPORT_SEPERATED_ETH_ITF),)
eth-objs += eth_virtual_if.o
endif
ifeq ($(strip $(TCSUPPORT_5_4_KERNEL) $(TCSUPPORT_5_10_KERNEL)),)
ifneq ($(TCSUPPORT_DS_HWNAT_OFFLOAD),)
eth-objs += ./soft_qdma/soft_qdma.o
endif
endif

ifneq ($(INCLUDE_ETH_SWITCH_RTL8372),)
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION
EXTRA_CFLAGS += -I$(KERNEL_DIR)/include
EXTRA_CFLAGS += -I$(OPENWRT_BUILD_ROOT)/ether/en7512/rtl8372n/
EXTRA_CFLAGS += -I$(OPENWRT_BUILD_ROOT)/ether/en7512/rtl8372n/dal/
EXTRA_CFLAGS += -I$(OPENWRT_BUILD_ROOT)/ether/en7512/rtl8372n/dal/rtl8373/

eth-objs += ./eth_rtl8372n.o
eth-objs += ./rtl8372n/acl.o
eth-objs += ./rtl8372n/chip.o
eth-objs += ./rtl8372n/cpuTag.o
eth-objs += ./rtl8372n/dos.o
eth-objs += ./rtl8372n/dot1x.o
eth-objs += ./rtl8372n/eee.o
eth-objs += ./rtl8372n/gpio.o
eth-objs += ./rtl8372n/i2c.o
eth-objs += ./rtl8372n/identify.o
eth-objs += ./rtl8372n/igmp.o
eth-objs += ./rtl8372n/interrupt.o
eth-objs += ./rtl8372n/isolation.o
eth-objs += ./rtl8372n/l2.o
eth-objs += ./rtl8372n/led.o
#eth-objs += ./rtl8372n/macsec.o
eth-objs += ./rtl8372n/mib.o
eth-objs += ./rtl8372n/miim.o
eth-objs += ./rtl8372n/mirror.o
eth-objs += ./rtl8372n/nic.o
eth-objs += ./rtl8372n/phy.o
eth-objs += ./rtl8372n/phy_rtl8224.o
eth-objs += ./rtl8372n/port.o
eth-objs += ./rtl8372n/ptp.o
eth-objs += ./rtl8372n/qos.o
eth-objs += ./rtl8372n/rate.o
eth-objs += ./rtl8372n/rldp.o
eth-objs += ./rtl8372n/rma.o
eth-objs += ./rtl8372n/rtk_switch.o
eth-objs += ./rtl8372n/sharemeter.o
eth-objs += ./rtl8372n/storm.o
eth-objs += ./rtl8372n/svlan.o
eth-objs += ./rtl8372n/trunk.o
eth-objs += ./rtl8372n/vlan.o
eth-objs += ./rtl8372n/Thermal_Meter.o
eth-objs += ./rtl8372n/dal/dal_mgmt.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_acl.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_cpuTag.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_dos.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_dot1x.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_drv.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_eee.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_fc.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_gpio.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_hsb.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_i2c.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_igmp.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_interrupt.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_isolation.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_led.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_lut.o
#eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_macsec.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_mapper.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_mib.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_mirror.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_nic.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_parser.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_phy.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_port.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_ptp.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_qos.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_rate.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_rma.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_rtkpp.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_sharemeter.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_storm.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_stp.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_svlan.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_switch.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_trunk.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_vlan.o
eth-objs += ./rtl8372n/dal/rtl8373/dal_rtl8373_wol.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_asicdrv.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_regField_list.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_reg_list.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_smi.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_tableField_list.o
eth-objs += ./rtl8372n/dal/rtl8373/rtl8373_table_list.o
endif


obj-m = eth.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko
ifeq ($(strip $(TCSUPPORT_5_4_KERNEL) $(TCSUPPORT_5_10_KERNEL)),)
ifneq ($(TCSUPPORT_DS_HWNAT_OFFLOAD),)
	$(call EcntBSPReleaseInstall,  $(ROOTPATH)/soft_qdma, soft_qdma.o, $(RELEASE_BSP_DIR)/$(TCPLATFORM)/modules/private/ether/en7512/soft_qdma)
endif	
endif	

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean


#eth-objs +=./tcphy/tcetherphy.o
