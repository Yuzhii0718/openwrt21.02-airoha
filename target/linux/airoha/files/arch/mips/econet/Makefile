obj-y := \
	prom.o \
	setup.o \
	irq.o \
	tcwdog.o \
	ex_mdio_api.o \
	ecnt_traps.o \
	../../../kernel/libcompileoption.o \
	ecnt_hook/ecnt_hook.o \
	ecnt_hook/ecnt_hook_nf.o \
	ecnt_kernel_ext.o
#	tcadsl.o 
#	ledcetrl.o

ifneq ($(strip $(TCSUPPORT_CPU_EN7580)),)
obj-y += package_info.o
endif

obj-$(CONFIG_MIPS_TC3162) += time.o
obj-$(CONFIG_MIPS_TC3162U) += time.o
obj-$(CONFIG_MIPS_TC3262) += time2.o
ifneq ($(strip $(TCSUPPORT_MIPS_1004K)),)
obj-$(CONFIG_MIPS_MT_SMP)	+= malta-amon.o
else
obj-$(CONFIG_MIPS_MT_SMP)	+= malta-smp.o
endif
obj-$(CONFIG_MIPS_MT_SMTC)	+= malta-smtc.o
obj-$(TCSUPPORT_KPROFILE)	+= kprofile_hook.o
ifneq ($(strip $(RTP_RX_SHORTCUT)),)
obj-$(TCSUPPORT_VOIP)	+= voip_hook.o
endif