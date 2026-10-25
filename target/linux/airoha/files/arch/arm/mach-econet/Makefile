ifeq ($(strip $(MT7510_EN7512_FPGA_STAGE)),) 
	obj-y := sgmii/
	ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
		obj-y += combo_phy/
	endif
endif
obj-y += \
	../../../kernel/libcompileoption.o \
	ecnt_hook/ecnt_hook.o \
	ecnt_hook/ecnt_hook_nf.o \
	ecnt_kernel_ext.o \
	ecnt_scu.o \
	prom.o \
	ecnt_sram.o \
	ecnt_gdma.o \
	ecnt_frame_engine.o \
	ecnt_ptp.o \
	ecnt_rbus.o \
	ecnt_npu.o \
	ecnt_xpon.o \
	ecnt_pon_phy.o \
	ecnt_crypto_k.o \
	ecnt_pcm.o \
	ecnt_cpu_top.o \
	ecnt_bootargs.o \
	ecnt_timer.o \
	ecnt_i2c_slave.o \
	ecnt_gdump.o \
	ecnt_irqbind.o \
	ecnt_i2s.o 

ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
	obj-y	+= ecnt_pbus_mon.o
endif	

obj-$(TCSUPPORT_KPROFILE)	+= kprofile_hook.o

ifneq ($(strip $(RTP_RX_SHORTCUT)),)
	obj-$(TCSUPPORT_VOIP)	+= voip_hook.o
endif

ifneq ($(findstring $(TCSUPPORT_CPU_AN7552)$(TCSUPPORT_CPU_EN7581),1 ),)
obj-y += thermal_avs/
else
obj-y += ecnt_scu_phy.o
ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
	obj-y += ecnt_thermal.o \
			 ecnt_avs.o	
endif
endif

ifeq ($(strip $(TCSUPPORT_CPU_ARMV8_64)),)
	obj-y	+= arm_v7_pmu.o
else
	obj-y	+= ecnt_ipsec.o ecnt_tod.o ecnt_pbus_mon.o
	obj-y 	+= combo_phy/
endif

ifneq ($(strip $(TCSUPPORT_ACTIVE_ETHERNET_WAN) $(TCSUPPORT_HSGMII_LAN)),)
	obj-y	+= ecnt_xsi.o
endif

ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
	obj-y	+= ecnt_trng.o \
			ecnt_i2c.o 
endif

#ifeq ($(strip $(TCSUPPORT_KERNEL_API) $(TCSUPPORT_OPENWRT)),)
ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
	obj-y	+= ecnt_gpio.o 
endif
