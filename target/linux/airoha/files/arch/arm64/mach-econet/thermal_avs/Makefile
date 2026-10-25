ifneq ($(TCSUPPORT_CPU_EN7581),)
obj-y += en7581/ecnt_scu_phy.o
ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
obj-y	+= en7581/ecnt_thermal.o \
en7581/ecnt_avs.o
endif
endif		
ifneq ($(TCSUPPORT_CPU_AN7552),)
obj-y += en7552/ecnt_scu_phy.o
ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
obj-y	+=	en7552/ecnt_thermal.o \
en7552/ecnt_avs.o
endif
endif