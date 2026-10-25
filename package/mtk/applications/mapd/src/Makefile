ifndef CC
CC=gcc
endif

#support for local compilation validation
#make ARCH=mips BUILD=local
ifeq ($(BUILD), local)
ifeq ($(ARCH),arm)
CC=/proj/mtk15451/toolchain-7622/bin/aarch64-openwrt-linux-gnu-gcc
endif

ifeq ($(ARCH),mips)
CC= /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc
endif
endif

ifndef CFLAGS
CFLAGS = -MMD -O2 -Wall -Werror -g
endif
CFLAGS += -Werror -Wall -g -fPIC

ifndef LDO
	LDO=$(CC)
endif

Q=@
E=echo
ifeq ($(V), 1)
	Q=
	E=true
endif
ifeq ($(QUIET), 1)
	Q=@
	E=true
endif

-include mapd_config
include $(TOPDIR)/.config

ifeq ($(CONFIG_TARGET_airoha),y)
CFLAGS += -Wno-address-of-packed-member
CFLAGS += -Wno-zero-length-bounds
CFLAGS += -Wno-error=unused-but-set-variable
endif

ifeq ($(CONFIG_MTK_EM_PLUS_SUPPORT),y)
CFLAGS += -DEM_PLUS_SUPPORT
endif

ifeq ($(CONFIG_MTK_WIFI7_MAP_VENDOR_SUPPORT),y)
CFLAGS += -DMAP_VENDOR_SUPPORT
CFLAGS += -DMAP_VENDOR_SUPPORT_RADAR
endif

# flag for uplink u32 backward compatible
CFLAGS += -DMAP_MLO_UPLINK

ifeq ($(CONFIG_MAP_R2_SUPPORT),y)
CFLAGS += -DMAP_R2
CFLAGS += -DDFS_CAC_R2
#CFLAGS += -DMAP_160BW
ifeq (y, $(filter y, $(CONFIG_MTK_MAP_R2_6E_SUPPORT) $(CONFIG_MTK_WIFI7_MAP_R2_6E_SUPPORT)))
CFLAGS += -DMAP_6E_SUPPORT
CFLAGS += -DMAP_RADIO_TEARDOWN
else
#flag added for non-6e controller in field with 6E agent
CFLAGS += -DMAP_NON6E_CONTROLLER
endif
ifeq ($(CONFIG_MTK_WIFI7_MAP_HOSTAPD_SUPPORT), y)
CFLAGS += -DMTK_HOSTAPD_SUPPORT
CFLAGS += -DMAP_EHT_SUPPORT
CFLAGS += -DMTK_MLO_MAP_SUPPORT
#enable flag for eagle faulty hardware for dedicated antenna BW80 CAC
ifeq ($(CONFIG_MAP_80M_ZW_DFS_SUPPORT), y)
CFLAGS += -DZW_DFS_BW80_ONLY
else
CFLAGS += -DZW_DFS_BW80_ONLY
endif
#flag added for not allowing 5G setup lnk MLO connection
CFLAGS += -DMAP_NO_5G_MLO
endif
endif
CFLAGS += -DACL_CTRL
CFLAGS += -DSINGLE_BAND_SUPPORT
ifeq ($(CONFIG_MAP_R3_SUPPORT),y)
CFLAGS += -DMAP_R3
CFLAGS += -DMAP_R3_RECONFIG
CFLAGS += -DMAP_R3_DPP_CONFIG
CFLAGS += -DMAP_R3_SP
#These flags are always on as per R4.
CFLAGS += -DMAP_R3_WF6
CFLAGS += -DMAP_R3_DE
CFLAGS += -DMAP_R4_SPT
endif

ifeq (y, $(filter y, $(CONFIG_MTK_DFS_SLAVE_SUPPORT) $(CONFIG_MTK_WIFI7_DFS_SLAVE_SUPPORT)))
CFLAGS += -DDFS_SLAVE_SUPPORT
endif

ifeq ($(CONFIG_MAP_R5_SUPPORT),y)
CFLAGS += -DMAP_R5
endif

ifeq ($(CONFIG_MTK_WIFI7_MAP_R6_SUPPORT),y)
CFLAGS += -DMAP_R6
endif
ifeq ($(CONFIG_MAP_R4_SUPPORT),y)
CFLAGS += -DMAP_R4
endif

#CFLAGS += -DBR_IP_TLV_SUPPORT

CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += -I$(abspath ./src)
CFLAGS += -I$(abspath ./src/utils)
CFLAGS += -I$(abspath ./src/dot11_if)
CFLAGS += -I$(abspath ./src/clientDB)
CFLAGS += -I$(abspath ./src/channelMon)
CFLAGS += -I$(abspath ./src/clientMon)
CFLAGS += -I$(abspath ./src/steerAction)
CFLAGS += -I$(abspath ./src/apEstimator)
CFLAGS += -I$(abspath ./src/ApRoamAlgo)
ifeq ($(CONFIG_MULTIAP),y)
CFLAGS += -I$(abspath ./src/ApCentStr)
CFLAGS += -I$(abspath ./src/topologySrv)
CFLAGS += -I$(abspath ./src/apSelection)
CFLAGS += -I$(abspath ./src/ch_planning/)
CFLAGS += -I$(abspath ./src/1905_if)
ifeq ($(CONFIG_MTK_WIFI7_MAP_R6_SUPPORT),y)
CFLAGS += -I$(abspath ./src/afc/)
endif
endif
CFLAGS += -I$(abspath ./)
CFLAGS += -I$(abspath ./mapd_interface/)
CFLAGS += -I$(abspath ../wappd/wapp_usr_intf/)
CFLAGS += -I$(abspath ../wappd/include/)
CFLAGS += -I$(abspath ./src/networkoptimization/)
CFLAGS += -I$(abspath ./src/mapfilter_if/)
ifeq ($(CONFIG_DATA_ELEMENT_SUPPORT),y)
CFLAGS += -I$(abspath ./src/data_element/)
endif
CFLAGS += -I$(abspath ./src/BlaT2lmAlgo/)
CFLAGS += -I$(abspath ./src/COSR/)
ifeq ($(CONFIG_1905_SUPPPORT),y)
FLAGS_1905 += -I$(abspath ../1905daemon/lib_export/)
else
CFLAGS += -I$(abspath ./1905_local_lib/)
endif

ifdef CONFIG_RELEASE_EXCLUDE
CFLAGS += -DRELEASE_EXCLUDE
endif

# VENDOR Flags begin
#CFLAGS += -DVENDOR1_FEATURE_EXTEND
# VENDOR Flags end

OBJS += ./src/utils/common.o
OBJS += ./src/utils/mapd_debug.o
OBJS += ./src/utils/os_unix.o
OBJS += ./src/utils/eloop.o
OBJS += ./src/steer_fsm.o
OBJS += ./src/clientDB/client_db.o
ifdef CONFIG_CLIENT_DB
CFLAGS += -DCONFIG_CLIENT_DB
ifeq ($(CONFIG_CLIENT_DB), file)
CFLAGS += -DCONFIG_CLIENT_DB_FILE
else ifeq ($(CONFIG_CLIENT_DB), nvram)
CFLAGS += -DCONFIG_CLIENT_DB_NVRAM
CFLAGS += -I$(ROOTDIR)/lib/libnvram
CFLAGS += -I$(abspath ../../../lib/libnvram/)
CFLAGS += -I$(abspath ../../../kernel_headers/include/)
LIBS += ../../../lib/libnvram/libnvram.a
LIBS_bs += ../../../lib/libnvram/libnvram.a
#LDFLAGS += -lnvram
endif
OBJS += ./src/clientDB/db_$(CONFIG_CLIENT_DB).o
endif

OBJS += ./src/channelMon/chan_mon.o
OBJS += ./src/clientMon/client_mon.o
OBJS += ./src/apEstimator/ap_est.o
OBJS += ./src/steerAction/steer_action.o
ifeq ($(CONFIG_MULTIAP),y)
OBJS += ./src/topologySrv/tlv_parsor.o
OBJS += ./src/topologySrv/topologySrv.o
OBJS += ./src/topologySrv/wappEvent.o
OBJS += ./src/topologySrv/metrics.o
OBJS += ./src/1905_if/1905_if.o
OBJS += ./src/apSelection/apSelection.o
OBJS += ./src/ch_planning/ch_planning.o
OBJS += ./src/ch_planning/ch_planning_r2.o
OBJS += ./src/ApCentStr/ap_cent_str.o
OBJS += ./src/mapfilter_if/mapfilter_if.o
OBJS += ./src/networkoptimization/network_optimization.o
ifeq ($(CONFIG_MTK_WIFI7_MAP_R6_SUPPORT),y)
LDFLAGS += -lcurl -ljson-c
endif
OBJS += ./src/COSR/mapd_cosr.o
endif
OBJS += main.o
OBJS += mapd.o
OBJS += ./mapd_interface/ctrl_iface.o ./mapd_interface/ctrl_iface_unix.o
OBJS += ./src/dot11_if/wapp_if.o
OBJS += ./src/BlaT2lmAlgo/BlaT2lmAlgo.o

LIBS += ../wappd/libwapp_usr_intf_client.a
LIBS += libmapd_core.a
LIBS += libmapd_steer_core.a
ifeq ($(CONFIG_1905_SUPPPORT),y)
ifeq ($(CONFIG_MTK_WIFI7_MAP_R6_SUPPORT),y)
OBJS += ./src/afc/mapd_afc.o
endif
LIBS += ../1905daemon/lib_export/lib1905_interface.a
endif
LIBS_bs += ../wappd/libwapp_usr_intf_client.a
LIBS_bs += libmapd_bs_core.a
LIBS_bs += libmapd_bs_steer_core.a
ifdef CONFIG_SUPPORT_OPENWRT
LIBS_c += -L ../libmapd -lmapd_interface_client
else

LIBS += -L ../../../lib/libmapd -lmapd_interface_client

endif #CONFIG_SUPPORT_OPENWRT

OBJS_c += mapd_ctrl/mapd_cli.o ./src/utils/common.o
ifeq ($(BUILD), local)
LIBS_c += /proj/mtk15451/mapd_lib/$(ARCH)/libwapp_usr_intf_client.a
else
LIBS_c +=  ../wappd/libwapp_usr_intf_client.a
endif

MAPD_LIB = libmapd_core.a
ifeq ($(CONFIG_MULTIAP_MTK_LIB),y)
OBJS_MAPD_LIB += mapd_lib/lib_score/lib_score.o
ifeq ($(CONFIG_MULTIAP),y)
OBJS_MAPD_LIB += mapd_lib/networkoptimization/network_optimization_lib.o
OBJS_MAPD_LIB += ./mapd_lib/topologySrv/topologySrv_lib.o
OBJS_MAPD_LIB += ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o
OBJS_MAPD_LIB += ./src/COSR/mapd_cosr.o
endif
else
OBJS_MAPD_LIB += ./mapd_lib/dummy_lib.o
endif

MAPD_STEER_LIB = libmapd_steer_core.a
ifeq ($(CONFIG_MULTIAP_MTK_STEER_LIB),y)
ifeq ($(CONFIG_MULTIAP_MTK_LIB),n)
OBJS_MAPD_STEER_LIB += mapd_lib/lib_score/lib_score.o
endif
OBJS_MAPD_STEER_LIB += ./mapd_lib/ApRoamAlgo/ap_roam_algo.o
OBJS_MAPD_STEER_LIB += ./mapd_lib/steerAction/steer_action_lib.o
OBJS_MAPD_STEER_LIB += ./mapd_lib/apEstimator/ap_est_lib.o
else
OBJS_MAPD_STEER_LIB += ./mapd_lib/dummy_steer_lib.o
endif
############BS#################
BS_LIB = libmapd_bs_core.a
ifeq ($(CONFIG_MULTIAP_MTK_LIB),y)
OBJS_BS_LIB += ./mapd_lib/lib_score/lib_score.o
OBJS_BS_LIB += ./mapd_lib/networkoptimization/network_optimization_lib.o
OBJS_BS_LIB += ./mapd_lib/topologySrv/topologySrv_lib.o
OBJS_BS_LIB += ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o
OBJS_BS_LIB += ./src/COSR/mapd_cosr.o
else
OBJS_BS_LIB += ./mapd_lib/dummy_lib.o
endif

BS_STEER_LIB = libmapd_bs_steer_core.a
ifeq ($(CONFIG_MULTIAP_MTK_STEER_LIB),y)
ifeq ($(CONFIG_MULTIAP_MTK_LIB),n)
OBJS_BS_STEER_LIB += mapd_lib/lib_score/lib_score.o
endif
OBJS_BS_STEER_LIB += ./mapd_lib/ApRoamAlgo/ap_roam_algo.o
OBJS_BS_STEER_LIB += ./mapd_lib/steerAction/steer_action_lib.o
OBJS_BS_STEER_LIB += ./mapd_lib/apEstimator/ap_est_lib.o
else
OBJS_BS_STEER_LIB += ./mapd_lib/dummy_steer_lib.o
endif
##########

ifeq ($(CONFIG_DATA_ELEMENT_SUPPORT),y)
CFLAGS += -DDATA_ELEMENT_SUPPORT
OBJS += ./src/data_element/base64.o
OBJS += ./src/data_element/cJSON.o
OBJS += ./src/data_element/de2json.o
endif
#############################
ifeq ($(CONFIG_MULTIAP),y)
all: clean version mapd mapd_cli standalone_bs mapd_user_iface
else
all: clean version standalone_bs bs_cli
endif
ifeq ($(CONFIG_MULTIAP),y)
CFLAGS += -DSUPPORT_MULTI_AP
ifeq ($(CONFIG_1905_SUPPPORT),y)
FLAGS_1905 += -DSUPPORT_1905
endif
ifeq ($(CONFIG_MULTIAP),y)
ifeq ($(CONFIG_AP_CENT_STR),y)
CFLAGS += -DCENT_STR
endif
endif
endif
ifeq ($(CONFIG_MULTIAP_MTK_STEER_LIB),n)
ifeq ($(CONFIG_MULTIAP_MTK_LIB),n)
CFLAGS += -DNO_SUPPORT_MULTIAP_MTK_LIB
endif
endif
ifdef CONFIG_CORE_DUMP
CFLAGS += -DCORE_DUMP_ENABLED
endif
#validated only for arm64, core dump should be used for normal execution
ifeq ($(CONFIG_BACKTRACK_SUPPORT),y)
CFLAGS += -funwind-tables -fasynchronous-unwind-tables -DBACKTRACK_SUPPORT
LDFLAGS = -rdynamic
endif
ifeq ($(CONFIG_DEBUG_SYSLOG),y)
CFLAGS += -DCONFIG_DEBUG_SYSLOG
endif

mapd_interface/mapd_interface_ctrl.o : mapd_interface/mapd_interface_ctrl.c 
	$(Q)$(LDO) -I src/utils -c -fPIC $^ -o $@
	
ifdef CONFIG_SUPPORT_OPENWRT
#src/utils/os_unix_lib.o : src/utils/os_unix.c
#	$(Q)$(LDO) -I src/utils -c -fPIC $^ -o $@
	
#libmapd_interface_client.so: mapd_interface/mapd_interface_ctrl.o src/utils/os_unix_lib.o
#	$(Q)$(LDO) -shared -o $@ $^
endif

version:
	@echo "#define current_version \"$$(date)\"" > ./version.h
	@$(E) "  LD " $@
ifeq ($(CONFIG_MULTIAP),y)
mapd: CFLAGS += $(FLAGS_1905)
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.c)","")
mapd: $(OBJS_MAPD_LIB) $(OBJS_MAPD_STEER_LIB) $(OBJS)
	ar r $(MAPD_LIB) $(OBJS_MAPD_LIB)
	ar r $(MAPD_STEER_LIB) $(OBJS_MAPD_STEER_LIB)
else
mapd: $(OBJS)
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.mapd)","")
####rename mapd based .o files ###
	cp -rf ./mapd_lib/lib_score/lib_score.o.mapd ./mapd_lib/lib_score/lib_score.o
	cp -rf ./mapd_lib/networkoptimization/network_optimization_lib.o.mapd ./mapd_lib/networkoptimization/network_optimization_lib.o
	cp -rf ./mapd_lib/topologySrv/topologySrv_lib.o.mapd ./mapd_lib/topologySrv/topologySrv_lib.o
	cp -rf ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o.mapd ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o
	cp -rf ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.mapd ./mapd_lib/ApRoamAlgo/ap_roam_algo.o
	cp -rf ./mapd_lib/steerAction/steer_action_lib.o.mapd ./mapd_lib/steerAction/steer_action_lib.o
	cp -rf ./mapd_lib/apEstimator/ap_est_lib.o.mapd ./mapd_lib/apEstimator/ap_est_lib.o
	cp -rf ./src/COSR/mapd_cosr.o.mapd ./src/COSR/mapd_cosr.o
endif
endif
	$(Q)$(LDO) $(LDFLAGS) -o mapd $(OBJS) $(LIBS) $(EXTRALIBS)
	@$(E) "  LD " $@
standalone_bs:
	mv mapd mapd_bk
	mv mapd_cli mapd_cli_bk
####taking backup of mapd related .o files
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.o)","")
	cp -rf ./mapd_lib/lib_score/lib_score.o ./mapd_lib/lib_score/lib_score.o.mapd
	cp -rf ./mapd_lib/networkoptimization/network_optimization_lib.o ./mapd_lib/networkoptimization/network_optimization_lib.o.mapd
	cp -rf ./mapd_lib/topologySrv/topologySrv_lib.o ./mapd_lib/topologySrv/topologySrv_lib.o.mapd
	cp -rf ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o.mapd
	cp -rf ./mapd_lib/ApRoamAlgo/ap_roam_algo.o ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.mapd
	cp -rf ./mapd_lib/steerAction/steer_action_lib.o ./mapd_lib/steerAction/steer_action_lib.o.mapd
	cp -rf ./mapd_lib/apEstimator/ap_est_lib.o ./mapd_lib/apEstimator/ap_est_lib.o.mapd
	cp -rf ./src/COSR/mapd_cosr.o ./src/COSR/mapd_cosr.o.mapd
endif
########
	make clean
	make bs20
	mv mapd_cli_bk mapd_cli
	mv mapd_bk mapd
####taking backup of BS related .o files #####
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.o)","")
	cp -rf ./mapd_lib/lib_score/lib_score.o ./mapd_lib/lib_score/lib_score.o.bs
	cp -rf ./mapd_lib/networkoptimization/network_optimization_lib.o ./mapd_lib/networkoptimization/network_optimization_lib.o.bs
	cp -rf ./mapd_lib/topologySrv/topologySrv_lib.o ./mapd_lib/topologySrv/topologySrv_lib.o.bs
	cp -rf ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o.bs
	cp -rf ./mapd_lib/ApRoamAlgo/ap_roam_algo.o ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.bs
	cp -rf ./mapd_lib/steerAction/steer_action_lib.o ./mapd_lib/steerAction/steer_action_lib.o.bs
	cp -rf ./mapd_lib/apEstimator/ap_est_lib.o ./mapd_lib/apEstimator/ap_est_lib.o.bs
	cp -rf ./src/COSR/mapd_cosr.o ./src/COSR/mapd_cosr.o.bs
endif
else
standalone_bs:
	make clean
	make bs20
endif

#ifeq ($(CONFIG_MULTIAP),y)
#bs20: CFLAGS += -I$(abspath ./src/1905_if)
#endif

#ifeq ($(CONFIG_1905_SUPPPORT),y)
#bs20: CFLAGS += $(FLAGS_1905)
#LIBS_bs += ../1905daemon/lib_export/lib1905_interface.a
#else
bs20: CFLAGS += -I$(abspath ./1905_local_lib/)
#endif

ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.c)","")
bs20:	$(OBJS_BS_LIB) $(OBJS_BS_STEER_LIB) $(OBJS)
	ar r $(BS_LIB) $(OBJS_BS_LIB)
	ar r $(BS_STEER_LIB) $(OBJS_BS_STEER_LIB)
else
bs20: $(OBJS)
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.bs)","")
####rename BS based .o files ###
	cp -rf  ./mapd_lib/lib_score/lib_score.o.bs ./mapd_lib/lib_score/lib_score.o
	cp -rf ./mapd_lib/networkoptimization/network_optimization_lib.o.bs ./mapd_lib/networkoptimization/network_optimization_lib.o
	cp -rf ./mapd_lib/topologySrv/topologySrv_lib.o.bs ./mapd_lib/topologySrv/topologySrv_lib.o
	cp -rf ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o.bs ./mapd_lib/ch_planning_r2/ch_planning_r2_lib.o
	cp -rf ./mapd_lib/ApRoamAlgo/ap_roam_algo.o.bs ./mapd_lib/ApRoamAlgo/ap_roam_algo.o
	cp -rf ./mapd_lib/steerAction/steer_action_lib.o.bs ./mapd_lib/steerAction/steer_action_lib.o
	cp -rf ./mapd_lib/apEstimator/ap_est_lib.o.bs ./mapd_lib/apEstimator/ap_est_lib.o
	cp -rf ./src/COSR/mapd_cosr.o.bs ./src/COSR/mapd_cosr.o
endif
endif
	$(Q)$(LDO) $(LDFLAGS) -o bs20 $(OBJS) $(LIBS_bs) $(EXTRALIBS)
	@$(E) "  LD " $@

mapd_lib_test: CFLAGS += -I$(abspath ./1905_local_lib/)
mapd_lib_test: $(OBJS_MAPD_LIB) $(OBJS_MAPD_STEER_LIB)
	ar r $(MAPD_LIB) $(OBJS_MAPD_LIB)
	ar r $(MAPD_STEER_LIB) $(OBJS_MAPD_STEER_LIB)
#	ar r $(BS_STEER_LIB) $(OBJS_BS_STEER_LIB)
#ifdef CONFIG_SUPPORT_OPENWRT
#mapd_cli: $(OBJS_c) libmapd_interface_client.so
#else
ifeq ($(CONFIG_MULTIAP),y)
mapd_cli: $(OBJS_c) 
#endif
	$(Q)$(LDO) $(LDFLAGS) -o mapd_cli $(OBJS_c) $(LIBS_c) $(EXTRALIBS_c) $(LIBS)
	@$(E) "  LD " $@
#ifeq ($(CONFIG_MULTIAP),y)
OBJS_u += user_iface_sample.o
mapd_user_iface: $(OBJS_u) 
	$(Q)$(LDO) $(LDFLAGS) -o mapd_user_iface $(OBJS_u) -lmapd_interface_client
else
bs_cli: $(OBJS_c)
#endif
	$(Q)$(LDO) $(LDFLAGS) -o bs_cli $(OBJS_c) $(LIBS_c) $(EXTRALIBS_c) $(LIBS_bs)
	@$(E) "  LD " $@
endif
romfs:
ifeq ($(CONFIG_MULTIAP),y)
	$(ROMFSINST) /sbin/mapd
	$(ROMFSINST) /sbin/mapd_cli
else
	$(ROMFSINST) /sbin/bs_cli
endif
	$(ROMFSINST) /sbin/bs20
	$(ROMFSINST) /sbin/EasyMesh.sh
ifeq ($(CONFIG_MULTIAP),y)
	$(ROMFSINST) /sbin/map_cert_script.sh
	$(ROMFSINST) /sbin/brsw-del.sh
endif
#ifdef CONFIG_SUPPORT_OPENWRT
#	$(ROMFSINST) /lib/libmapd_interface_client.so
#endif
	$(ROMFSINST) /etc_ro/wts_bss_info_config
clean:
	$(MAKE) -C ./src clean
	$(MAKE) -C ./mapd_ctrl clean
	$(MAKE) -C ./mapd_interface clean
ifdef CONFIG_SUPPORT_OPENWRT
#	rm -f mapd_cli mapd libmapd_interface_client.so
endif
ifeq ($(CONFIG_MULTIAP),y)
	rm -f mapd_cli mapd bs20
else
	rm -f bs_cli bs20
endif
ifneq ("$(wildcard ./mapd_lib/ApRoamAlgo/ap_roam_algo.c)","")
	rm -f core *~ *.o *.d *.gcno *.gcda *.gcov
	rm -f $(OBJS_MAPD_LIB)
	rm -f $(OBJS_MAPD_STEER_LIB)
	rm -f $(OBJS_BS_LIB)
	rm -f $(OBJS_BS_STEER_LIB)
	rm -f $(ALL)
else
	rm -f mapd.o user_iface_sample.o main.o
	rm -f core *~ *.d *.gcno *.gcda *.gcov
endif
