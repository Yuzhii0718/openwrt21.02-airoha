#
# Makefile for ARM-specific library files..
#
obj-y := softfloat.o

LIB2FUNCS_1 = _fixunsdfdi _fixdfdi _fixsfdi _fixunssfdi

# These might cause a divide overflow trap and so are compiled with
# unwinder info.
LIB2_DIVMOD_FUNCS = _divdi3 _moddi3 _udivdi3 _umoddi3 _udiv_w_sdiv _udivmoddi4 _ffsdi2

LIB_ARM32_FUNCS = _arm_negsf2 _arm_addsubsf3 _arm_muldivsf3 _arm_cmpsf2 _arm_unordsf2 _arm_fixsfsi _arm_fixunssfsi \
	_arm_negdf2 _arm_addsubdf3 _arm_muldivdf3 _arm_cmpdf2 _arm_unorddf2 _arm_fixdfsi _arm_fixunsdfsi _arm_truncdfsf2 \
	_aeabi_uldivmod _aeabi_ldivmod _dvmd_tls _aeabi_lcmp 

ARM32_OBJS := $(patsubst %,%.o, $(LIB_ARM32_FUNCS))

GCC_OBJS := $(patsubst %,%.o, $(LIB2FUNCS_1)) $(patsubst %,%.o, $(LIB2FUNCS_2)) $(patsubst %,%.o, $(LIB2_DIVMOD_FUNCS))
softfloat-objs := $(DP_OBJS) $(FP_OBJS) $(GCC_OBJS) softfloat_sym.o $(ARM32_OBJS) bpabi_new.o

# ARM32 soft-float library add
$(addprefix $(obj)/,$(patsubst %,%.o, $(LIB_ARM32_FUNCS))): $(obj)/lib1funcs.S
	$(CC) -D__ASSEMBLY__ $(c_flags) -c $(NOSTDINC_FLAGS) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -DFINE_GRAINED_LIBRARIES -DL$(notdir $(patsubst %.o,%, $@)) $(obj)/lib1funcs.S -o $@

$(addprefix $(obj)/,$(patsubst %,%.o, $(LIB2FUNCS_1))): $(obj)/libgcc2.c
	$(CC) $(c_flags) -c $(NOSTDINC_FLAGS) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -DFINE_GRAINED_LIBRARIES -DL$(notdir $(patsubst %.o,%, $@)) $(obj)/libgcc2.c -o $@

$(addprefix $(obj)/,$(patsubst %,%.o, $(LIB2FUNCS_2))): $(obj)/libgcc2.c
	$(CC) $(c_flags) -c $(NOSTDINC_FLAGS) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -DFINE_GRAINED_LIBRARIES -DL$(notdir $(patsubst %.o,%, $@)) $(obj)/libgcc2.c -o $@
$(addprefix $(obj)/,$(patsubst %,%.o, $(LIB2_DIVMOD_FUNCS))): $(obj)/libgcc2.c
	$(CC) $(c_flags) -c $(NOSTDINC_FLAGS) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -DFINE_GRAINED_LIBRARIES -DL$(notdir $(patsubst %.o,%, $@)) $(obj)/libgcc2.c -o $@	
