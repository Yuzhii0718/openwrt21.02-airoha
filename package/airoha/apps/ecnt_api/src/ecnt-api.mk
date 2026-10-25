export BSP_PRIVATEDIR=$(BSP_ROOTDIR)/private
export BSP_PUBLICDIR=$(BSP_ROOTDIR)/public
export BSP_API_DIR=$(BSP_PRIVATEDIR)/API

ifneq ($(strip $(TCSUPPORT_OPENWRT)),)
export APP_BSP_DIR=$(KERNEL_BUILD_DIR)/ecnt_api
export BSP_API_DIR=$(APP_BSP_DIR)
endif

###################################################################
# path define of hw_driver  (hardware APIs)
export BSP_API_HW_DRIVER_DIR=$(BSP_API_DIR)/hw_driver

export ECNT_EVENT_LIB_DIR=$(BSP_API_HW_DRIVER_DIR)/ecnt_event
export BSP_API_DBG_LIB_DIR=$(BSP_API_HW_DRIVER_DIR)/ecnt_lib_dbg
export BSP_API_UTL_LIB_DIR=$(BSP_API_HW_DRIVER_DIR)/ecnt_lib_utility
export BSP_API_ECNT_PRINTF_DIR=$(BSP_API_HW_DRIVER_DIR)/ecnt_lib_printf

ifneq ($(strip $(TCSUPPORT_OPENWRT)),)
LIB_INSTALL_DIR:=$(APP_BSP_DIR)/bsp_lib
FILESYSTEM_DIR:=$(APP_BSP_DIR)/bsp_fs
LIBINSTALL_LDFLAGS:=-L$(LIB_INSTALL_DIR)
LIBINSTALL_CFLAGS:=-I$(LIB_INSTALL_DIR)
APP_BSP_LIB_DIR:=$(LIB_INSTALL_DIR)
export APP_API_LIB_OUTPUT_DIR:=$(LIB_INSTALL_DIR)
export APP_BSP_API_LIB_OUTPUT_DIR:=$(APP_BSP_LIB_DIR)
else
#only wifi  & mtkitf use need fixed later
endif

export BSP_API_ATM_LIB_DIR=$(BSP_API_DIR)/hw_driver/atm/lib_src
export BSP_API_ATM_CMD_DIR=$(BSP_API_DIR)/hw_driver/atm/cmd
export BSP_API_PTM_LIB_DIR=$(BSP_API_DIR)/hw_driver/ptm/lib_src
export BSP_API_PTM_CMD_DIR=$(BSP_API_DIR)/hw_driver/ptm/cmd
export BSP_API_XDSL_LIB_DIR=$(BSP_API_DIR)/hw_driver/xdsl/lib_src
export BSP_API_XDSL_CMD_DIR=$(BSP_API_DIR)/hw_driver/xdsl/cmd
export BSP_API_FE_LIB_DIR=$(BSP_API_DIR)/hw_driver/fe/lib_src
export BSP_API_FE_LIBBSP_DIR=$(BSP_API_DIR)/hw_driver/fe/libbsp_src
export BSP_API_FE_CMD_DIR=$(BSP_API_DIR)/hw_driver/fe/cmd
export BSP_API_SWITCH_LIB_DIR=$(BSP_API_DIR)/hw_driver/switch/lib_src
export BSP_API_SWITCH_LIBBSP_DIR=$(BSP_API_DIR)/hw_driver/switch/libbsp_src
export BSP_API_SWITCH_CMD_DIR=$(BSP_API_DIR)/hw_driver/switch/cmd
export BSP_API_PPE_LIB_DIR=$(BSP_API_DIR)/hw_driver/ppe/lib_src
export BSP_API_PPE_LIBBSP_DIR=$(BSP_API_DIR)/hw_driver/ppe/libbsp_src
export BSP_API_PPE_CMD_DIR=$(BSP_API_DIR)/hw_driver/ppe/cmd
export BSP_API_NPU_CMD_DIR=$(BSP_API_DIR)/hw_driver/npu/cmd
export BSP_API_QDMA_LIB_DIR=$(BSP_API_DIR)/hw_driver/qdma/lib_src
export BSP_API_QDMA_LIBBSP_DIR=$(BSP_API_DIR)/hw_driver/qdma/libbsp_src
export BSP_API_QDMA_CMD_DIR=$(BSP_API_DIR)/hw_driver/qdma/cmd
export BSP_API_HW_NAT_CMD_DIR=$(BSP_API_DIR)/hw_driver/hw_nat/cmd
export BSP_API_QOS_LIB_QOSMGR_DIR = $(BSP_API_DIR)/hw_driver/qos/lib_qosmgr
export BSP_API_QOS_LIB_FLOWMGR_DIR = $(BSP_API_DIR)/hw_driver/qos/lib_flowmgr
export BSP_API_QOS_CMD_QOSMGR_DIR = $(BSP_API_DIR)/hw_driver/qos/cmd_qosmgr
export BSP_API_QOS_CMD_FLOWMGR_DIR = $(BSP_API_DIR)/hw_driver/qos/cmd_flowmgr
export BSP_API_WIFI_LIB_DIR=$(BSP_API_DIR)/hw_driver/wifi/lib_src
export BSP_API_WIFI_CMD_DIR=$(BSP_API_DIR)/hw_driver/wifi/cmd
export BSP_API_CPU_INTERRUPT_LIB_DIR=$(BSP_API_DIR)/hw_driver/cpu_interrupt/lib_src
export BSP_API_CPU_INTERRUPT_CMD_DIR=$(BSP_API_DIR)/hw_driver/cpu_interrupt/cmd
export BSP_API_DBG_DIR=$(BSP_API_DIR)/hw_driver/dbgcmd/cmd
export BSP_API_LIB_VDSL_DIR=$(BSP_API_DIR)/hw_driver/vdsl/lib_src
export BSP_API_LIB_DBG_ATM_DIR=$(BSP_API_DIR)/hw_driver/dbgcmd/lib_src/atm/
export BSP_API_QOSRULE_LIB_DIR=$(BSP_API_DIR)/hw_driver/qosrule/lib_src
export BSP_API_QOSRULE_CMD_DIR=$(BSP_API_DIR)/hw_driver/qosrule/cmd
export BSP_API_IFC_LIB_DIR=$(BSP_API_DIR)/hw_driver/ifc/lib_src
export BSP_API_IFC_CMD_DIR=$(BSP_API_DIR)/hw_driver/ifc/cmd
export BSP_API_SPEEDTESTV2_DIR=$(BSP_API_DIR)/hw_driver/speedtest_v2/

#for wifi
export BSP_MESH_DIR=$(BSP_API_DIR)/hw_driver/mesh

#for xpon
export BSP_API_ECNT_IGMP_LIB_DIR=$(BSP_API_DIR)/hw_driver/ecnt_igmp/lib_src
export BSP_API_ECNT_IGMP_CMD_DIR=$(BSP_API_DIR)/hw_driver/ecnt_igmp/cmd

export BSP_API_CPUTEMP_LIB_DIR = $(BSP_API_DIR)/hw_driver/cpu/lib_src
export BSP_API_CPUTEMP_CMD_DIR = $(BSP_API_DIR)/hw_driver/cpu/cmd
export BSP_API_CMD_UART_DIR=$(BSP_API_DIR)/hw_driver/uart/cmd
export BSP_PROLINECMD_DIR=$(BSP_API_DIR)/hw_driver/ecnt_lib_prolinecmd
###################################################################
# path define of sw_api  (software APIs)
export BSP_API_SW_API_DIR=$(BSP_API_DIR)/sw_api
export BSP_API_ENV_LIB_DIR=$(BSP_API_SW_API_DIR)/ecnt_lib_env
export BSP_API_PORTBIND_LIB_DIR=$(BSP_API_SW_API_DIR)/ecnt_lib_portbind

###################################################################
# path define of blapi
export BSP_BLAPI_DIR=$(BSP_API_DIR)/ecnt_blapi


export BSP_API_MTK_ITF_DIR=$(BSP_API_DIR)/mtk_itf
export BSP_API_MTK_ITF_QOS_DIR = $(BSP_API_MTK_ITF_DIR)/mtk_itf_qos
export BSP_API_MTK_ITF_SWITCH_DIR = $(BSP_API_MTK_ITF_DIR)/mtk_itf_switch
export BSP_API_MTK_ITF_FE_DIR=$(BSP_API_MTK_ITF_DIR)/mtk_itf_fe
export BSP_API_MTK_ITF_SIMULATION_DIR=$(BSP_API_MTK_ITF_DIR)/mtk_itf_simulation
export BSP_API_MTK_ITF_PPE_DIR=$(BSP_API_MTK_ITF_DIR)/mtk_itf_ppe

export BSP_API_SISM_DIR = $(BSP_API_DIR)/hw_driver/service_manager

export BSP_API_PCIE_LIB_DIR=$(BSP_API_DIR)/hw_driver/pcie/lib_src
export BSP_API_PCIE_CMD_DIR=$(BSP_API_DIR)/hw_driver/pcie/cmd

#BSP INT API for BSP internel api
BSPAPI_INT_INCDIR=$(BSP_INT_INC)
export BSPAPI_INTCFLAGS=$(patsubst %,-I%,$(subst :, ,$(BSPAPI_INT_INCDIR)))

BSPAPP_INT_LIBDIR=$(BSP_INT_LIB)
export BSPAPI_INTLDFLAGS = $(patsubst %,-L%,$(subst :, ,$(BSPAPP_INT_LIBDIR))) -Wl,-rpath-link,$(BSPAPP_INT_LIBDIR)

#BSP API for blapi/or other custom sdk
BSPAPI_INCDIR=$(BSP_INT_INC):$(BSP_EXT_INC)
export BSPAPI_CFLAGS=$(patsubst %,-I%,$(subst :, ,$(BSPAPI_INCDIR)))

BSPAPI_LIBDIR=$(BSP_EXT_LIB):$(BSP_INT_LIB)
export BSPAPI_LDFLAGS = $(patsubst %,-L%,$(subst :, ,$(BSPAPI_LIBDIR))) -Wl,-rpath-link,$(BSPAPI_LIBDIR)
