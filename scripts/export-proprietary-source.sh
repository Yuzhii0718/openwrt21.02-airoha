#!/bin/sh
target_arch="arm_cortex-a7"
BUILD_DIR="build_dir/target-arm_cortex-a7_musl_eabi"
KERNEL_BUILD_DIR="${BUILD_DIR}/linux-airoha_en7563"
DEST_DIR=$(dirname $(dirname "$PWD")) #生成的文件存放在当前目录的上上级目录中
echo $DEST_DIR
opkgPath="bin/packages/$target_arch/base"

kernelPackPath="package/kernelpack1"
appPackPath="package/apppack1"

mkdir -p ${DEST_DIR}/target_src
TARGET_DIR="${DEST_DIR}/target_src"

kernelPackPath="${TARGET_DIR}/${kernelPackPath}"
appPackPath="${TARGET_DIR}/${appPackPath}"

mkdir -p ${kernelPackPath}/files
[ -d "$kernelPackPath" ] && rm -rf $kernelPackPath

mkdir -p ${appPackPath}/files
[ -d "$appPackPath" ] && rm -rf $appPackPath


kernel_paths=(
    "package/mtk/drivers/mt_hwifi/Makefile"						#be included in mt_wifi7
	"package/mtk/drivers/mt_wifi_cmn/Makefile"
	"package/mtk/drivers/mt_wifi7/Makefile"
	"package/mtk/drivers/warp/Makefile"
	"package/feeds/airoha/ae_wan/Makefile" 							#"feeds/airoha/package/airoha/drivers/ae_wan/Makefile"
	"package/feeds/airoha/airoha_gre_hw_offload/Makefile" 			#"feeds/airoha/package/airoha/drivers/airoha_gre_hw_offload/Makefile"
	"package/feeds/airoha/airoha_l2tp_offload/Makefile" 			#"feeds/airoha/package/airoha/drivers/airoha_l2tp_offload/Makefile"
	"package/feeds/airoha/airoha_pptp_offload/Makefile" 			#"feeds/airoha/package/airoha/drivers/airoha_pptp_offload/Makefile"
	"package/feeds/airoha/airoha_vxlan_hw_offload/Makefile" 		#"feeds/airoha/package/airoha/drivers/airoha_vxlan_hw_offload/Makefile"
	"package/feeds/airoha/ether/Makefile" 							#"feeds/airoha/package/airoha/drivers/ether/Makefile"
	"package/feeds/airoha/fe/Makefile" 								#"feeds/airoha/package/airoha/drivers/fe/Makefile"
	"package/feeds/airoha/hw_nat/Makefile" 							#"feeds/airoha/package/airoha/drivers/hw_nat/Makefile"
	"package/feeds/airoha/ifc/Makefile" 							#"feeds/airoha/package/airoha/drivers/ifc/Makefile"
	"package/feeds/airoha/lro/Makefile" 							#"feeds/airoha/package/airoha/drivers/lro/Makefile"
	"package/feeds/airoha/npu/Makefile" 							#"feeds/airoha/package/airoha/drivers/npu/Makefile"
	"package/feeds/airoha/qdma/Makefile" 							#"feeds/airoha/package/airoha/drivers/qdma/Makefile"
	"package/feeds/airoha/sif/Makefile" 							#"feeds/airoha/package/airoha/drivers/sif/Makefile"
	"package/feeds/airoha/tc3162l2hp2h/Makefile" 					#"feeds/airoha/package/airoha/drivers/tc3162l2hp2h/Makefile"
	"package/feeds/airoha/tcci/Makefile" 							#"feeds/airoha/package/airoha/drivers/tcci/Makefile"
	"package/feeds/airoha/tcphy/Makefile" 							#"feeds/airoha/package/airoha/drivers/tcphy/Makefile"
)

app_paths=(
    "package/libs/mtk_libnl-tiny/Makefile"
	"package/mtk/applications/ated_ext/Makefile"
	#"package/mtk/applications/datconf/Makefile"					#too hard
	#"package/mtk/applications/luci-app-mtk/Makefile"
	#"package/mtk/applications/mtk-base-files/Makefile"
	"package/mtk/applications/mwctl/Makefile"
	"package/mtk/applications/sigma_daemon/Makefile"
	"package/mtk/applications/sigma_dut_v9.0.0/Makefile"
	"package/mtk/applications/wificonf/Makefile"
    "package/network/utils/mtk_iw/Makefile"
	"package/network/utils/mtk_iwinfo/Makefile"
    #"feeds/airoha/tools/ecnt_trx/Makefile"
	"package/mtk/drivers/wifi-profile/Makefile"
	"package/feeds/airoha/airoha_network/Makefile" 					#"feeds/airoha/package/airoha/apps/airoha_network/Makefile"
	"package/feeds/airoha/compileoption_lib/Makefile" 				#"feeds/airoha/package/airoha/apps/compileoption_lib/Makefile"
	"package/feeds/airoha/ecnt_api/Makefile" 						#"feeds/airoha/package/airoha/apps/ecnt_api/Makefile"
	"package/feeds/airoha/ecnt_ated/Makefile" 						#"feeds/airoha/package/airoha/apps/ecnt_ated/Makefile"
	"package/feeds/airoha/ecnt_bsp/Makefile" 						#"feeds/airoha/package/airoha/apps/ecnt_bsp/Makefile"
	"package/feeds/airoha/ecnt_prepare/Makefile" 					#"feeds/airoha/package/airoha/apps/ecnt_prepare/Makefile"
	"package/feeds/airoha/ecnt_sys/Makefile" 						#"feeds/airoha/package/airoha/apps/ecnt_sys/Makefile"
	"package/feeds/airoha/ecnt_uci2dat/Makefile" 					#"feeds/airoha/package/airoha/apps/ecnt_uci2dat/Makefile"
	"package/feeds/airoha/ecnt_wan/Makefile" 						#"feeds/airoha/package/airoha/apps/ecnt_wan/Makefile"
	"package/feeds/airoha/ecnt_wl_e2p_chk/Makefile" 				#"feeds/airoha/package/airoha/apps/ecnt_wl_e2p_chk/Makefile"
	"package/feeds/airoha/ethcmd/Makefile" 							#"feeds/airoha/package/airoha/apps/ethcmd/Makefile"
	"package/feeds/airoha/hwnat_7510/Makefile" 						#"feeds/airoha/package/airoha/apps/hwnat_7510/Makefile"
	"package/feeds/airoha/iperf3_airoha/Makefile" 					#"feeds/airoha/package/airoha/apps/iperf3_airoha/Makefile"
	"package/feeds/airoha/ratemanager/Makefile" 					#"feeds/airoha/package/airoha/apps/ratemanager/Makefile"
)

function build_package() {
    makefile=$1
    funcname=$2
    destFile=$3
    local localstate=0
        #for line in `cat $makefile`
    cat $makefile | while read line
    do
        ret=`echo $line | awk '$1=="define" {print $1}'`
        ret2=`echo $line | awk '$1=="define" {print $2}'`
        ret3=`echo $line | awk '$1=="endef" {print $1}'`

        if [ "$ret" == "define" ];then
            packName=$funcname
            if [ "$ret2" == "$packName" ];then
                #echo "" >> $destFile
                #echo $line >> $destFile
                localstate=1
                continue
            fi
        fi

        if [ "$ret3" == "endef" ] && [ $localstate -eq 1 ];then
            #echo $line >> $destFile
            localstate=0
            break
        fi

        if [ $localstate -eq 1 ];then
            funcname=`echo "${line}" | grep -Eo "\\\\$\\(.*\\)"`
            if [ "$funcname" != "" ];then
                funcname=${line##*(}
                funcname=${funcname%*)}
                build_package $makefile $funcname $destFile
            fi
            echo -e "\t$line" >> $destFile
        fi
    done
}

for makefile in "${kernel_paths[@]}"
do
    echo makefile:$makefile
    PKG_NAMETEMP=`grep PKG_NAME:= -r $makefile | tr -d "\r"`
	echo PKG_NAMETEMP:$PKG_NAMETEMP
    PKG_NAMETEMP=${PKG_NAMETEMP##*=}
	echo PKG_NAMETEMP:$PKG_NAMETEMP
	PKG_NAME=`echo "$PKG_NAMETEMP" | sed 's/.*$(ECNT)//'`
	echo PKG_NAME:$PKG_NAME
    PKG_RELEASE=`grep PKG_RELEASE:= -r -r $makefile | tr -d "\r"`
    PKG_RELEASE=${PKG_RELEASE##*=}
    PKG_VERSION=`grep PKG_VERSION:= -r -r $makefile | tr -d "\r"`
    PKG_VERSION=${PKG_VERSION##*=}
	PKG_BUILD_DIR=`grep PKG_BUILD_DIR:= -r -r $makefile | tr -d "\r"`
    PKG_BUILD_DIR=${PKG_BUILD_DIR##*=}
    if [ $PKG_NAME == "tcboot" ];then
        PKG_SOURCE_VERSION=""
        echo "skip tcboot"
        continue
    else
        PKG_SOURCE_VERSION=`grep PKG_SOURCE_VERSION:= -r -r $makefile | tr -d "\r"`
        PKG_SOURCE_VERSION=${PKG_SOURCE_VERSION##*=}
    fi
    PKG_SOURCE_DATE=`grep PKG_SOURCE_DATE:= -r -r $makefile | tr -d "\r"`
    PKG_SOURCE_DATE=${PKG_SOURCE_DATE##*=}

    if [ -z "$PKG_VERSION" ];then
        PKG_VERSION=$PKG_SOURCE_DATE
    fi
	
	if [ -z "$PKG_VERSION" ];then
        PKG_VERSION="1"
    fi
    echo PKG_NAME:$PKG_NAME PKG_RELEASE:$PKG_RELEASE PKG_VERSION:=$PKG_VERSION
    #IPK_NAME=kmod-${PKG_NAME}

    #echo "IPK_NAME:"$IPK_NAME
    destFile=`echo $makefile | sed 's/package//'`
    destFile=${kernelPackPath}${destFile}
    destDir=${destFile%/*}
    echo "destFile:"$destFile $destDir
    mkdir -p ${destDir}
    mkdir -p ${destDir}/files

	if [ ! -d ${KERNEL_BUILD_DIR}/${PKG_NAME}/ipkg-${target_arch} ];then
        echo "$PKG_NAME has no source"
		continue
    fi

    echo tar -zcvf ${destDir}/files/data.tar.gz -C ${KERNEL_BUILD_DIR}/${PKG_NAME}/ipkg-${target_arch}/
    tar -zcvf ${destDir}/files/data.tar.gz -C ${KERNEL_BUILD_DIR}/${PKG_NAME}/ipkg-${target_arch}/ .
    echo "hello:$(find ${KERNEL_BUILD_DIR}/${PKG_NAME} -name "*.h")"
    [ "$(find ${KERNEL_BUILD_DIR}/${PKG_NAME} -depth -maxdepth 1 -name "*.h")" != "" ] && cp ${KERNEL_BUILD_DIR}/${PKG_NAME}/*.h ${destDir}/files/
    [ "$(find ${KERNEL_BUILD_DIR}/${PKG_NAME}/ -depth -maxdepth 1 -name "*.so")" != "" ] && cp ${KERNEL_BUILD_DIR}/${PKG_NAME}/*.so ${destDir}/files/
    [ -d ${KERNEL_BUILD_DIR}/${PKG_NAME}/include ] && cp -rf ${KERNEL_BUILD_DIR}/${PKG_NAME}/include ${destDir}/files/
    [ -f ${KERNEL_BUILD_DIR}/${PKG_NAME}/${PKG_NAME} ] && cp ${KERNEL_BUILD_DIR}/${PKG_NAME}/${PKG_NAME} ${destDir}/files/

    echo "include "\$\(TOPDIR\)"/rules.mk" > $destFile
	echo "include \$(INCLUDE_DIR)/kernel.mk" >> $destFile
	echo "include \$(INCLUDE_DIR)/ecnt-release.mk" >> $destFile
    echo "PKG_NAME:=${PKG_NAMETEMP}" >> $destFile
    if [ -n "$PKG_RELEASE" ];then
        echo "PKG_RELEASE:=${PKG_RELEASE}" >> $destFile
    fi

    if [ -n "$PKG_VERSION" ];then
        echo "PKG_VERSION:=${PKG_VERSION}" >> $destFile
    fi

    if [ -n "$PKG_SOURCE_VERSION" ];then
        echo "PKG_SOURCE_VERSION:=${PKG_SOURCE_VERSION}" >> $destFile
    fi

    if [ -n "$PKG_SOURCE_DATE" ];then
        echo "PKG_SOURCE_DATE:=${PKG_SOURCE_DATE}" >> $destFile
    fi
	
	if [ -n "$PKG_BUILD_DIR" ];then
        echo "PKG_BUILD_DIR:=${PKG_BUILD_DIR}" >> $destFile
    fi   

    echo "include \$(INCLUDE_DIR)/package.mk" >> $destFile
    
    state=0
    #for line in `cat $makefile`
    extro_content=""
    cat $makefile | while read line
    do
        ret=`echo $line | awk '$1=="define" {print $1}' | tr -d "\r"`
        ret2=`echo $line | awk '$1=="define" {print $2}' | tr -d "\r"`
        ret3=`echo $line | tr -d "\r" `
        #ret2=`echo ${ret2} | sed 's/\$(PKG_NAME)/${PKG_NAME}/'`
        ret2=`echo ${ret2} | sed "s/\\$(PKG_NAME)/${PKG_NAME}/"`
        line=`echo -e $line | tr -d "\r"`

        if [ "$ret" == "define" ];then
            packName="KernelPackage/${PKG_NAME}"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo $line >> $destFile
                state=1
                continue
            else
                echo "packName:$packName"
                echo "ret:$ret2"
            fi
            
            packName="KernelPackage/${PKG_NAME}/description"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo $line >> $destFile
                state=1
                continue
            fi

            packName="KernelPackage/${PKG_NAME}/extra_provides"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo -e "$line" >> $destFile
                extro_content=""
                state=3
                continue
            fi

            if [ "$ret2" == "Build/InstallDev" ]; then
                echo "" >> $destFile
                echo -e "$line" >> $destFile
                state=2
                continue
            fi
        fi
        
        if [ "$ret3" == "endef" ] && [ $state -eq 1 ];then
            echo -e "$line" >> $destFile
            state=0
            continue
        fi

        if [ "$ret3" == "endef" ] && [ $state -eq 2 ];then
            echo -e "$line" >> $destFile
            state=0
            continue
        fi
        
        if [ "$ret3" == "endef" ] && [ $state -eq 3 ];then
            echo -e "\t$extro_content" >> $destFile
            echo -e "$line" >> $destFile
            state=0
            continue
        fi

        if [ $state -eq 1 ];then
            funcname=`echo "${line}" | grep -Eo "\\\\$\\(.*\\)"`
            if [ "$funcname" != "" ];then
                funcname=${funcname##*(}
                funcname=${funcname%*)}
                build_package $makefile $funcname $destFile
                continue
            fi
            echo -e "  $line" >> $destFile
        fi

        if [ $state -eq 2 ];then
            echo -e "\t$line" >> $destFile
        fi

        if [ $state -eq 3 ];then
            extro_content="${extro_content}$line"
        fi
    done

    echo "define Build/Prepare" >> $destFile
    echo -e "\tmkdir -p \$(PKG_BUILD_DIR)" >> $destFile
    echo -e "\t\$(CP) ./src/* \$(PKG_BUILD_DIR)" >> $destFile
    echo "${KERNEL_BUILD_DIR}/${PKG_NAME}/*.h"
    if [ "$(find ${KERNEL_BUILD_DIR}/${PKG_NAME} -depth -maxdepth 1 -name "*.h")" != "" ];then
        echo -e "\t\$(CP) ./files/*.h \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -d ${KERNEL_BUILD_DIR}/${PKG_NAME}/include ];then
        echo -e "\t[ -d ./files/include ] && \$(CP) -rf ./files/include \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ "$(find ${KERNEL_BUILD_DIR}/${PKG_NAME}/ -depth -maxdepth 1 -name "*.so")" != "" ];then
        echo -e "\t\$(CP) ./files/*.so \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -e ${KERNEL_BUILD_DIR}/${PKG_NAME}/${PKG_NAME} ];then
        echo -e "\t[ ./files/${PKG_NAME} ] && \$(CP) ./files/${PKG_NAME} \$(PKG_BUILD_DIR)" >> $destFile
    fi

    echo "endef" >> $destFile

    #echo "define Build/InstallDev" >> $destFile
    #echo -e "\techo install" >> $destFile
    #echo "endef" >> $destFile
    #echo "" >> $destFile

    echo "define KernelPackage/${PKG_NAME}/install" >> $destFile
    echo -e "\ttar -zxvf files/data.tar.gz -C \$(1)/../" >> $destFile
    echo "endef" >> $destFile
    
    echo "\$(eval \$(call KernelPackage,${PKG_NAME}))" >> $destFile 
    
    mkdir -p ${destDir}/src
    echo "all:" > ${destDir}/src/Makefile
    echo -e "\techo "hello"" >> ${destDir}/src/Makefile

    mkdir -p $kernelPackPath
	
	origin_dir="${makefile%/*}"
	echo origin_dir:$origin_dir
	rm -rf ${TARGET_DIR}/${origin_dir}
	cp -rf ${destDir} ${TARGET_DIR}/${origin_dir}
done

for makefile in "${app_paths[@]}"
do
    echo makefile:$makefile
    PKG_NAME=`grep PKG_NAME:= -r $makefile | tr -d "\r"`
    PKG_NAME=${PKG_NAME##*=}
    PKG_RELEASE=`grep PKG_RELEASE:= -r -r $makefile | tr -d "\r"`
    PKG_RELEASE=${PKG_RELEASE##*=}
    PKG_VERSION=`grep PKG_VERSION:= -r -r $makefile | tr -d "\r"`
    PKG_VERSION=${PKG_VERSION##*=}
    PKG_SOURCE_VERSION=`grep PKG_SOURCE_VERSION:= -r -r $makefile | tr -d "\r"`
    PKG_SOURCE_VERSION=${PKG_SOURCE_VERSION##*=}
    PKG_SOURCE_DATE=`grep PKG_SOURCE_DATE:= -r -r $makefile | tr -d "\r"`
    PKG_SOURCE_DATE=${PKG_SOURCE_DATE##*=}
	PKG_BUILD_DIR=`grep PKG_BUILD_DIR:= -r -r $makefile | tr -d "\r"`
    PKG_BUILD_DIR=${PKG_BUILD_DIR##*=}

	IsKernelPackage=`grep KernelPackage -r -r $makefile | tr -d "\r"`
	if [ -z "$IsKernelPackage" ];then
		if [[ "$PKG_BUILD_DIR" == *"KERNEL_BUILD_DIR"* ]];then
			IsKernelPackage="1"
		fi
	fi
	
    if [ -z "$PKG_VERSION" ];then
        PKG_VERSION=$PKG_SOURCE_DATE
    fi
	if [ -z "$PKG_VERSION" ];then
        PKG_VERSION="1"
    fi

    echo PKG_NAME:$PKG_NAME PKG_RELEASE:$PKG_RELEASE PKG_VERSION:=$PKG_VERSION IsKernelPackage:=$IsKernelPackage
    #IPK_NAME=kmod-${PKG_NAME}

    #echo "IPK_NAME:"$IPK_NAME
    destFile=`echo $makefile | sed 's/package//'`
    destFile=${appPackPath}${destFile}
    destDir=${destFile%/*}
    echo "destFile:"$destFile $destDir
    mkdir -p ${destDir}
    mkdir -p ${destDir}/files

	VERSION_BUILD=""
	if [ -n "$PKG_SOURCE_DATE" ]; then
        if [ -n "$PKG_SOURCE_VERSION" ]; then
			VERSION_BUILD=-${PKG_SOURCE_DATE}-${PKG_SOURCE_VERSION:0:8}
		fi
	fi
	echo "VERSION_BUILD:"$VERSION_BUILD
	
	Package_BUILD_DIR="${BUILD_DIR}/${PKG_NAME}${VERSION_BUILD}"
	if [ -n "$IsKernelPackage" ]; then
		Package_BUILD_DIR="${KERNEL_BUILD_DIR}/${PKG_NAME}"
	fi
	
	if [ ! -d ${Package_BUILD_DIR}/ipkg-${target_arch} ];then
        echo "$PKG_NAME has no source"
		continue
    fi
	
	echo tar -zcvf ${destDir}/files/data.tar.gz -C ${Package_BUILD_DIR}/ipkg-${target_arch}/
	tar -zcvf ${destDir}/files/data.tar.gz -C ${Package_BUILD_DIR}/ipkg-${target_arch}/ .

	echo "hello:$(find ${Package_BUILD_DIR} -name "*.h")"
	[ "$(find ${Package_BUILD_DIR} -depth -maxdepth 1 -name "*.h")" != "" ] && cp ${Package_BUILD_DIR}/*.h ${destDir}/files/
	[ "$(find ${Package_BUILD_DIR}/ -depth -maxdepth 1 -name "*.so")" != "" ] && cp ${Package_BUILD_DIR}/*.so ${destDir}/files/
	[ -d ${Package_BUILD_DIR}/include ] && cp -rf ${Package_BUILD_DIR}/include ${destDir}/files/
	[ -e ${Package_BUILD_DIR}/${PKG_NAME} ] && cp ${Package_BUILD_DIR}/${PKG_NAME} ${destDir}/files/

    echo "include "\$\(TOPDIR\)"/rules.mk" > $destFile
	echo "include \$(INCLUDE_DIR)/kernel.mk" >> $destFile
	echo "include \$(INCLUDE_DIR)/ecnt-release.mk" >> $destFile
    echo "PKG_NAME:=${PKG_NAME}" >> $destFile
    if [ -n "$PKG_RELEASE" ];then
        echo "PKG_RELEASE:=${PKG_RELEASE}" >> $destFile
    fi

    if [ -n "$PKG_VERSION" ];then
        echo "PKG_VERSION:=${PKG_VERSION}" >> $destFile
    fi

    if [ -n "$PKG_SOURCE_VERSION" ];then
        echo "PKG_SOURCE_VERSION:=${PKG_SOURCE_VERSION}" >> $destFile
    fi

    if [ -n "$PKG_SOURCE_DATE" ];then
        echo "PKG_SOURCE_DATE:=${PKG_SOURCE_DATE}" >> $destFile
    fi    

	if [ -n "$PKG_BUILD_DIR" ];then
        echo "PKG_BUILD_DIR:=${PKG_BUILD_DIR}" >> $destFile
    fi 

    echo "include \$(INCLUDE_DIR)/package.mk" >> $destFile
    
    state=0
    #for line in `cat $makefile`
    extro_content=""
    cat $makefile | while read line
    do
        ret=`echo $line | awk '$1=="define" {print $1}' | tr -d "\r"`
        ret2=`echo $line | awk '$1=="define" {print $2}' | tr -d "\r"`
        ret3=`echo $line | tr -d "\r" `
        #ret2=`echo ${ret2} | sed 's/\$(PKG_NAME)/${PKG_NAME}/'`
        ret2=`echo ${ret2} | sed "s/\\$(PKG_NAME)/${PKG_NAME}/"`
        line=`echo -e $line | tr -d "\r"`

        if [ "$ret" == "define" ];then
            packName="Package/${PKG_NAME}"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo $line >> $destFile
                state=1
                continue
            else
                echo "packName:$packName"
                echo "ret:$ret2"
            fi
            
            packName="Package/${PKG_NAME}/description"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo $line >> $destFile
                state=1
                continue
            fi

            packName="Package/${PKG_NAME}/extra_provides"
            if [ "$ret2" == "$packName" ];then
                echo "" >> $destFile
                echo -e "$line" >> $destFile
                extro_content=""
                state=3
                continue
            fi

            if [ "$ret2" == "Build/InstallDev" ]; then
                echo "" >> $destFile
                echo -e "$line" >> $destFile
                state=2
                continue
            fi
        fi
        
        if [ "$ret3" == "endef" ] && [ $state -eq 1 ];then
            echo -e "$line" >> $destFile
            state=0
            continue
        fi

        if [ "$ret3" == "endef" ] && [ $state -eq 2 ];then
            echo -e "$line" >> $destFile
            state=0
            continue
        fi
        
        if [ "$ret3" == "endef" ] && [ $state -eq 3 ];then
            echo -e "\t$extro_content" >> $destFile
            echo -e "$line" >> $destFile
            state=0
            continue
        fi

        if [ $state -eq 1 ];then
            funcname=`echo "${line}" | grep -Eo "\\\\$\\(.*\\)"`
            if [ "$funcname" != "" ];then
                funcname=${funcname##*(}
                funcname=${funcname%*)}
                build_package $makefile $funcname $destFile
                continue
            fi
            echo -e "\t$line" >> $destFile
        fi

        if [ $state -eq 2 ];then
            echo -e "\t$line" >> $destFile
        fi

        if [ $state -eq 3 ];then
            extro_content="${extro_content}$line"
        fi
    done

    echo "define Build/Prepare" >> $destFile
    echo -e "\tmkdir -p \$(PKG_BUILD_DIR)" >> $destFile
    echo -e "\t\$(CP) ./src/* \$(PKG_BUILD_DIR)" >> $destFile
    echo "${Package_BUILD_DIR}/*.h"
    if [ "$(find ${Package_BUILD_DIR} -depth -maxdepth 1 -name "*.h")" != "" ];then
        echo -e "\t\$(CP) ./files/*.h \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -d ${Package_BUILD_DIR}/include ];then
        echo -e "\t[ -d ./files/include ] && \$(CP) -rf ./files/include \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ "$(find ${Package_BUILD_DIR}/ -depth -maxdepth 1 -name "*.so")" != "" ];then
        echo -e "\t\$(CP) ./files/*.so \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -f ${Package_BUILD_DIR}/${PKG_NAME} ];then
        echo -e "\t[ ./files/${PKG_NAME} ] && \$(CP) ./files/${PKG_NAME} \$(PKG_BUILD_DIR)" >> $destFile
    fi

    echo "endef" >> $destFile

    echo "define Package/${PKG_NAME}/install" >> $destFile
    echo -e "\ttar -zxvf files/data.tar.gz -C \$(1)/../" >> $destFile
    echo "endef" >> $destFile
    
    echo "\$(eval \$(call BuildPackage,${PKG_NAME}))" >> $destFile 
    
    mkdir -p ${destDir}/src
    echo "all:" > ${destDir}/src/Makefile
    echo -e "\techo "hello"" >> ${destDir}/src/Makefile

    mkdir -p $appPackPath
	
	origin_dir="${makefile%/*}"
	echo origin_dir:$origin_dir
	rm -rf ${TARGET_DIR}/${origin_dir}
	cp -rf ${destDir} ${TARGET_DIR}/${origin_dir}
done


#bootloader
#PKG_NAME=tcboot
#destFile=${kernelPackPath}/${PKG_NAME}/Makefile
#destDir=${destFile%/*}
#mkdir -p ${destDir}/files
#mkdir -p ${destDir}/tcboot
#mkdir -p ${destDir}/src
#echo "all:" > ${destDir}/src/Makefile
#echo -e "\techo "hello"" >> ${destDir}/src/Makefile
#echo "destFile:"$destFile $destDir
#tar -zcvf ${destDir}/files/data.tar.gz -C ${KERNEL_BUILD_DIR}/${PKG_NAME}/ipkg-${target_arch}/ .
#cp -rf ${KERNEL_BUILD_DIR}/${PKG_NAME}/tcboot ${destDir}/
#cp -rf ${KERNEL_BUILD_DIR}/${PKG_NAME}/tools ${destDir}/
#cp -rf ${KERNEL_BUILD_DIR}/${PKG_NAME}/src/Uboot/u-boot-2014.04-rc1/env.bin ${destDir}/tcboot/
#rm -rf ${kernelPackPath}/feeds/airoha/bootloader 
#mv ${kernelPackPath}/tcboot/  ${kernelPackPath}/feeds/airoha/bootloader/
#rm -rf ${TARGET_DIR}/package/feeds/airoha/bootloader
#cp -rf ${kernelPackPath}/feeds/airoha/bootloader ${TARGET_DIR}/package/feeds/airoha/

#special behavior
#1. ecnt_api
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/blapi*.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/libapi_lib*.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/ecnt_lib_dbg.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/fe_type.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/hwnat_ioctl.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/libecntevent.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/include/trx.h ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/lib/libblapi*.so* ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/lib/libapi_lib*.so* ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/
cp -rf staging_dir/target-arm_cortex-a7_musl_eabi/usr/lib/libecnt*.so* ${TARGET_DIR}/package/feeds/airoha/ecnt_api/src/

#2. npu
rsync -avm --include='*/' --include='*.h' --exclude='*' ${KERNEL_BUILD_DIR}/npu/UNION_EN7563_MULTI_CHIP_KERNEL_5_4_demo/ ${TARGET_DIR}/package/feeds/airoha/npu/src/UNION_EN7563_MULTI_CHIP_KERNEL_5_4_demo/

#3. mtk_libnl-tiny
cp -rf ${BUILD_DIR}/libnl-tiny-2023-12-05-965c4bf4/ipkg-install/ ${TARGET_DIR}/package/libs/mtk_libnl-tiny/src/
cp -rf ${BUILD_DIR}/libnl-tiny-2023-12-05-965c4bf4/libnl-tiny.pc ${TARGET_DIR}/package/libs/mtk_libnl-tiny/src/

#4. mt_hwifi
cp -rf ${TARGET_DIR}/package/mtk/drivers/mt_wifi7/files/ ${TARGET_DIR}/package/mtk/drivers/mt_hwifi/files/

#remove proprietary code
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/apps/*
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/ae_wan/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/airoha_gre_hw_offload/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/airoha_l2tp_offload/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/airoha_pptp_offload/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/airoha_vxlan_hw_offload/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/ether/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/fe/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/hw_nat/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/ifc/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/lro/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/npu/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/qdma/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/sif/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/tc3162l2hp2h/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/tcci/
rm -rf ${TARGET_DIR}/feeds/airoha/package/airoha/drivers/tcphy/

rm -rf ${TARGET_DIR}/dl/ated_ext*
rm -rf ${TARGET_DIR}/dl/mt7992_20*
rm -rf ${TARGET_DIR}/dl/sigma_daemon*
rm -rf ${TARGET_DIR}/dl/sigma_dut*
rm -rf ${TARGET_DIR}/dl/warp*
rm -rf ${TARGET_DIR}/dl/wificonf*

#remove temp file
rm -rf ${kernelPackPath}
rm -rf ${appPackPath}
