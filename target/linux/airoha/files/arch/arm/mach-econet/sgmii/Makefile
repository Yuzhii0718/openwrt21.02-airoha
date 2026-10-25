ifneq ($(strip $(TCSUPPORT_CPU_EN7581)),)
obj-y += air_eth_xsgmii.o
obj-y += air_xpon_xsgmii.o
obj-y += air_usb_xsgmii.o
obj-y += air_pcie2l_xsgmii.o
obj-y += air_xsgmii_proc.o
obj-y += air_xsgmii_api.o
else ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
obj-y += an7552/air_xpon_xsgmii.o
obj-y += an7552/air_xsgmii_proc.o
obj-y += an7552/air_xsgmii_api.o
else
# EN7523 profile
ifneq ($(strip $(TCSUPPORT_ACTIVE_ETHERNET_WAN) $(TCSUPPORT_HSGMII_LAN)),)
obj-y += ecnt_pon_hsgmii.o
#obj-y += ecnt_pc0_hsgmii.o
#obj-y += ecnt_pc1_hsgmii.o
#obj-y += ecnt_usb_hsgmii.o
obj-y += ecnt_sgmii.o
obj-y += sgmii_cmd.o
#obj-y += sgmii_api.o
endif
endif
