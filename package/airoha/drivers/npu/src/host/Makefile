KERNELDIR=$(KERNEL_DIR)

obj-m += npu.o
npu-objs := npu_rv32.o 

ifeq ($(NPU_TEST_CODE),1)
npu-objs += ../TestCode/host/npu_host_test.o
ifneq ($(TCSUPPORT_CPU_ARMV8), 1)
npu-objs += ../TestCode/host/npuSramTest.o
endif
endif

EXTRA_CFLAGS += -I$(GLOBAL_INC_DIR)/modules

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

