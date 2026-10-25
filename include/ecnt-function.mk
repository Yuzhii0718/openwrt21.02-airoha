
define EcntInstall
#	@echo install files:{$(2)} in $(1) to $(3)
	$(foreach fname,$(strip $(2)),cp -rf $(1)/$(fname) $(3);)
endef

define EcntunInstall
	@echo uninstall files:{$(2)} in $(1) to $(3)
	-cd $(3) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define BLApiLibInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_LIB)
	$(foreach fname,$(strip $(2)),cp -rf $(1)/$(fname) $(BSP_EXT_LIB);)
	$(if $(strip $(3)), \
		$(foreach fname, $(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_LIB);) )
endef

define BLApiIncInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_INC)
	$(foreach fname,$(strip $(2)),cp -rf $(1)/$(fname) $(BSP_EXT_INC);)
	$(if $(strip $(3)), \
		$(foreach fname, $(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_INC);) )
endef
#MAKEVAR=$3
define BLApiBinInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_FS_USEFS)	
	$(foreach fname,$(strip $(2)), cp -rf $(1)/$(fname) $(BSP_EXT_FS_USEFS);)
	$(if $(strip $(3)), \
		$(foreach fname, $(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_FS_USEFS);) )
endef

define BLApiBinClean
	@echo clean: $(1) $(BSP_EXT_FS_USEFS)
	-cd $(BSP_EXT_FS_USEFS) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
	-cd $(BSP_INT_FS_USEFS) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define BLApiLibClean
	@echo clean: $(1) $(BSP_EXT_LIB)
	$(foreach fname, $(strip $(1)), find $(BSP_EXT_LIB) -name '$(fname)' | xargs rm  -rf;)
endef

define BLApiIncClean
	@echo clean: $(1) $(BSP_EXT_INC)
	$(foreach fname, $(strip $(1)), find $(BSP_EXT_INC) -name '$(fname)' | xargs rm  -rf;) 
endef

define BLApiMake
	@echo @@@@@@@@@ make $(1) $(2) MAKEVAR=$@
	$(MAKE) -C $(1) $(2) MAKEVAR=$(findstring $(strip $@), $(INSTALL_TARGET))
endef

define EcntApiLibInstall
	$(foreach fname, $(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_LIB) )
	$(if $(strip $3), $(call EcntInstall, $(1), $(2), $(BSP_EXT_LIB)) )
endef

define EcntApiIncInstall
	$(call EcntInstall, $(1), $(2), $(BSP_INT_INC))
	$(if $(strip $3), $(call EcntInstall, $(1), $(2), $(BSP_EXT_INC)) )
endef


define EcntApiBinInstall
	@echo install files:{$(2)} in $(1) to $(BSP_INT_FS_USEFS)	
	$(foreach fname,$(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_FS_USEFS);)
	$(if $(strip $3), $(call EcntInstall, $(1), $(2), $(BSP_EXT_FS_USEFS)) )
endef

define EcntApiBinClean
	-cd $(BSP_INT_FS_USEFS) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define EcntApiUsrBinInstall
	@echo install files:{$(2)} in $(1) to $(BSP_INT_FS_USR)	
	$(foreach fname,$(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_FS_USR);)
	$(if $(strip $3), $(call EcntInstall, $(1), $(2), $(BSP_EXT_FS_USR)) )
endef

define EcntApiUsrBinClean
	-cd $(BSP_INT_FS_USR) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define EcntApiUsrEtcInstall
	@echo install files:{$(2)} in $(1) to $(BSP_INT_FS_USR)/usr/etc/	
	$(foreach fname,$(strip $(2)), cp -rf $(1)/$(fname) $(BSP_INT_FS)/usr/etc/;)
	$(if $(strip $3), $(call EcntInstall, $(1), $(2), $(BSP_INT_FS)/usr/etc/) )
endef

define EcntMake
	@echo @@@@@@@@@ make $(1) $(2)
	$(MAKE) -C $(1) $(2)
endef

#$(call EcntMake, $(1), $(2), $(3)) 
define EcntApiMake
	@echo @@@@@@@@@ make $(1) $(2) MAKEVAR=$(findstring $(strip $@), $(INSTALL_TARGET))
	$(MAKE) -C $(1) $(2) MAKEVAR=$(findstring $(strip $@), $(INSTALL_TARGET))
endef
#	$(if $(findstring $(3), $(INSTALL_TARGET)), $(call EcntMake, $(1), $(EXT_INSTALL_TARGERT)) )

define EcntApiLibClean
	@echo clean: $(1) $(BSP_INT_LIB)/ $(BSP_EXT_LIB)
	-cd $(BSP_INT_LIB) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;)
	-cd $(BSP_EXT_LIB) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;)
endef

define EcntApiIncClean
	@echo clean: $(1) $(BSP_INT_INC)
	-cd $(BSP_INT_INC) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
	-cd $(BSP_EXT_INC) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define EcntCleanKo
	@echo clean: $(1) $(FILESYSTEM_DIR)/ $(FILESYSTEM_BSP_DIR)
	-cd $(FILESYSTEM_DIR) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;)
	-cd $(FILESYSTEM_BSP_DIR) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;)
endef

define BspAppMake	
	@echo @@@@@@@@@ make $(1) $(2) MAKEVAR=$(findstring $(strip $@), $(INSTALL_TARGET))
	$(MAKE) -C $(1) $(2) MAKEVAR=$(findstring $(strip $@), $(INSTALL_TARGET))
endef

#	@echo install files:{$(2)} in $(1) to $(3)
define BspAppBinInstall
	$(foreach fname,$(strip $(2)), cp -rf $(1)/$(fname) $(3);)
endef

define BspAppBinClean
	@echo BspAppBinClean
	-cd $(2) && $(foreach fname, $(strip $(1)), find ./ -name '$(fname)' | xargs rm  -rf;) 
endef

define BspAppUserFsInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_FS_USEFS_DIR)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXT_FS_USEFS_DIR))
endef

define BspAppUserFsBinInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_FS_USEFS)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXT_FS_USEFS))
endef

define BspAppUserFsBinClean
	@echo BspAppUserFsBinClean
	$(call BspAppBinClean, $(1), $(BSP_EXT_FS_USEFS))
endef

define BspAppUsrBinInstall
	@echo install files:{$(2)} in $(1) to $(BSP_EXT_FS_USR)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXT_FS_USR))
endef

define BspAppUsrBinClean
	@echo BspAppUsrBinClean
	$(call BspAppBinClean, $(1), $(BSP_EXT_FS_USR))
endef

define BspExclUsrBinInstall
	@echo BspExclUsrBinInstall files:{$(2)} in $(1) to $(BSP_EXCL_FS_USR)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXCL_FS_USR))
endef

define BspExclUsrBinClean	
	$(call BspAppBinClean, $(1), $(BSP_EXT_FS_USEFS))
endef

define BspExclUserFsBinInstall
	@echo BspExclUserFsBinInstall files:{$(2)} in $(1) to $(BSP_EXCL_FS_USEFS_BIN)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXCL_FS_USEFS_BIN))
endef

define BspExclUserFsBinClean	
	$(call BspAppBinClean, $(1), $(BSP_EXCL_FS_USEFS_BIN))
endef

define BspExclUserFsInstall
	@echo BspExclUserFsBinInstall files:{$(2)} in $(1) to $(BSP_EXCL_FS_USEFS)	
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXCL_FS_USEFS))
endef

define BspExclUserFsClean	
	$(call BspAppBinClean, $(1), $(BSP_EXCL_FS_USEFS))
endef

define BspExclFsLibInstall
	@echo BspExclFsLibInstall files:{$(2)} in $(1) to $(BSP_EXCL_FS)/lib
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXCL_FS)/lib)
endef

define BspExclFsLibClean
	$(call BspAppBinClean, $(1), $(BSP_EXCL_LIB))
endef

define BspExclIncInstall
	$(call EcntInstall, $(1), $(2), $(BSP_EXCL_INC))
endef

define BspExclIncClean
	$(call EcntunInstall, $(1), $(2), $(BSP_EXCL_INC))
endef

define BspExclUsrEtcInstall
	@echo BspExclUsrEtcInstall files:{$(2)} in $(1) to $(BSP_EXCL_FS)/usr/etc/
	$(call BspAppBinInstall, $(1), $(2), $(BSP_EXCL_FS)/usr/etc)
endef

define EcntModuleClean
	@echo @@@@@@ EcntModuleClean $(1)
	$(MAKE) $(TC_PARALLEL_BUILD_PARAM) -C $(1) clean
	-cd $(BSP_EXT_MODULE) && $(foreach koname, $(strip $(2)), find ./ -name '$(koname)' | xargs rm  -rf;)
	-cd $(1) && $(foreach name, $(strip $(1)), find ./ -name Module.symvers | xargs rm  -rf;)
endef

define EcntModuleMake
	@echo @@@@@ EcntModuleMake $(1)
	$(MAKE) $(TC_PARALLEL_BUILD_PARAM) -C $(1) $(3)
	$(foreach koname, $(strip $(2)), cp -rf $(1)/$(koname) $(BSP_EXT_MODULE);)
endef

define EcntModuleInstall
	@echo @@@@@ EcntModuleInstall $(1)
	$(foreach koname, $(strip $(2)), cp -rf $(1)/$(koname) $(BSP_EXT_MODULE);)
endef

define EcntBSPReleaseInstall
	@echo @@@@@ EcntBSPReleaseInstall $(1)
	$(if $(strip $(SDKRelease)), \
		$(shell if [ ! -d $(3) ]; then install -d $(3); fi;) \
		$(foreach fname, $(strip $(2)), cp -rf $(1)/$(fname) $(3);) )
endef

define EcntBSPReleaseUsrfsBinInstall
	@echo @@@@@ EcntBSPReleaseUsrfsBinInstall $(1)
	$(call EcntBSPReleaseInstall, $(1), $(2), $(RELEASE_BSP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin)
endef

define EcntBSPReleaseUsrfsInstall
	@echo @@@@@ EcntBSPReleaseUsrfsInstall $(1)
	$(call EcntBSPReleaseInstall, $(1), $(2), $(RELEASE_BSP_DIR)/$(TCPLATFORM)/filesystem/userfs)
endef

define EcntBSPReleaseUsrBinInstall
	@echo @@@@@ EcntBSPReleaseUsrBinInstall $(1)
	$(call EcntBSPReleaseInstall, $(1), $(2), $(RELEASE_BSP_DIR)/$(TCPLATFORM)/filesystem/usr/bin)
endef

define EcntBSPReleaseLibInstall
	@echo @@@@@ EcntBSPReleaseLibInstall $(1)
	$(call EcntBSPReleaseInstall, $(1), $(2), $(RELEASE_BSP_DIR)/$(TCPLATFORM)/filesystem/lib)
endef

define EcntBSPReleaseLibModulesInstall
	@echo @@@@@ EcntBSPReleaseLibModulesInstall $(1)
	$(call EcntBSPReleaseInstall, $(1), $(2), $(RELEASE_BSP_DIR)/$(TCPLATFORM)/filesystem/lib/modules)
endef