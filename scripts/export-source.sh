#!/bin/sh
target_arch="arm_cortex-a7"
BUILD_DIR="build_dir/target-arm_cortex-a7_musl_eabi"
DEST_DIR=$(dirname $(dirname "$PWD")) #生成的文件存放在当前目录的上上级目录中
echo $DEST_DIR
opkgPath="bin/packages/$target_arch/base"

packPath="package/twpack1"

mkdir -p ${DEST_DIR}/target_src
TARGET_DIR="${DEST_DIR}/target_src"

packPath="${TARGET_DIR}/${packPath}"

mkdir -p ${packPath}/files
[ -d "$packPath" ] && rm -rf $packPath



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

for makefile in `ls package/tw/*/*/Makefile`
do
    echo makefile:$makefile
    PKG_NAME=`grep PKG_NAME:= -r $makefile | tr -d "\r"`
    PKG_NAME=${PKG_NAME##*=}
    PKG_RELEASE=`grep PKG_RELEASE:= -r -r $makefile | tr -d "\r"`
    PKG_RELEASE=${PKG_RELEASE##*=}
    PKG_VERSION=`grep PKG_VERSION:= -r -r $makefile | tr -d "\r"`
    PKG_VERSION=${PKG_VERSION##*=}
    if [ $PKG_NAME == rae ];then
        PKG_SOURCE_VERSION=""
        echo "skip rae mode"
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
    echo PKG_NAME:$PKG_NAME PKG_RELEASE:$PKG_RELEASE PKG_VERSION:=$PKG_VERSION
    IPK_NAME=${PKG_NAME}

    VERSION=""
    VERSION_BUILD=""
    if [ -n "$PKG_VERSION" ]; then
        if [ -n "$PKG_RELEASE" ]; then
            if [ -n "$PKG_SOURCE_VERSION" ];then
                VERSION=${PKG_VERSION}-${PKG_SOURCE_VERSION:0:8}-${PKG_RELEASE}
                VERSION_BUILD=-${PKG_VERSION}-${PKG_SOURCE_VERSION:0:8}
            else
                VERSION=${PKG_VERSION}-${PKG_RELEASE}
                VERSION_BUILD=-${PKG_VERSION}
            fi 
        else
            VERSION=${PKG_VERSION}
            VERSION_BUILD=-${PKG_VERSION}
        fi
    else
        if [ -n "$PKG_RELEASE" ]; then
            VERSION=${PKG_VERSION}
        fi
    fi
    
    if [ -n "$VERSION" ]; then
        VERSION="_"${VERSION}
    fi
    echo "IPK_NAME:"$IPK_NAME
    #cp $opkgPath/${IPK_NAME}${VERSION}_${target_arch}.ipk ${packPath}/files
    destFile=`echo $makefile | sed 's/package\/tw//'`
    destFile=${packPath}${destFile}
    destDir=${destFile%/*}
    echo "destFile:"$destFile $destDir
    mkdir -p ${destDir}
    mkdir -p ${destDir}/files

    echo tar -zcvf ${destDir}/files/data.tar.gz -C ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/ipkg-${target_arch}/${IPK_NAME}
    tar -zcvf ${destDir}/files/data.tar.gz -C ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/ipkg-${target_arch}/${IPK_NAME} .
    echo "hello:$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD} -name "*.h")"
    [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD} -depth -maxdepth 1 -name "*.h")" != "" ] && cp ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/*.h ${destDir}/files/
    [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/ -depth -maxdepth 1 -name "*.so")" != "" ] && cp ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/*.so ${destDir}/files/
    [ -d ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/include ] && cp -rf ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/include ${destDir}/files/
    [ -e ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/${PKG_NAME} ] && cp ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/${PKG_NAME} ${destDir}/files/
#The following instruction are only applicable to GPL compilation error issues
    [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD} -depth -maxdepth 1 -name "libnvram.a")" != "" ] && cp ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/* ${destDir}/files/

    echo "include "\$\(TOPDIR\)"/rules.mk" > $destFile
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
    echo "${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/*.h"
    if [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD} -depth -maxdepth 1 -name "*.h")" != "" ];then
        echo -e "\t\$(CP) ./files/*.h \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -d ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/include ];then
        echo -e "\t[ -d ./files/include ] && \$(CP) -rf ./files/include \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/ -depth -maxdepth 1 -name "*.so")" != "" ];then
        echo -e "\t\$(CP) ./files/*.so \$(PKG_BUILD_DIR)" >> $destFile
    fi

    if [ -e ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/${PKG_NAME} ];then
        echo -e "\t[ ./files/${PKG_NAME} ] && \$(CP) ./files/${PKG_NAME} \$(PKG_BUILD_DIR)" >> $destFile
    fi

#The following instructions are only applicable to GPL compilation error issues
    if [ "$(find ${BUILD_DIR}/${IPK_NAME}${VERSION_BUILD}/ -depth -maxdepth 1 -name "libnvram.a")" != "" ];then
        echo -e "\t\$(CP) ./files/* \$(PKG_BUILD_DIR)" >> $destFile
    fi

    echo "endef" >> $destFile

    #echo "define Build/InstallDev" >> $destFile
    #echo -e "\techo install" >> $destFile
    #echo "endef" >> $destFile
    #echo "" >> $destFile

    echo "define Package/${PKG_NAME}/install" >> $destFile
    echo -e "\ttar -zxvf files/data.tar.gz -C \$(1)/" >> $destFile
    echo "endef" >> $destFile
    
    echo "\$(eval \$(call BuildPackage,${PKG_NAME}))" >> $destFile 
    
    mkdir -p ${destDir}/src
    echo "all:" > ${destDir}/src/Makefile
    echo -e "\techo "hello"" >> ${destDir}/src/Makefile

    mkdir -p $packPath
done


#create opensource dir
mkdir -p ${DEST_DIR}/target_src
for dir in autobuild BSDmakefile config Config.in feeds.conf.default include key-build key-build.pub Makefile package README.md rules.mk scripts target toolchain tools .config files feeds LICENSES
do
    cp -rf $dir ${TARGET_DIR}
done

rm -rf ${TARGET_DIR}/package/tw/applications
mv  ${packPath}/applications ${TARGET_DIR}/package/tw/

#rm -rf ${TARGET_DIR}/target/linux/mediatek/patches-5.4/*
#rm -rf ${TARGET_DIR}/package/mtk/drivers/mt_wifi/patches/*
rm -rf ${TARGET_DIR}/package/mtk/applications/fwdd/patches/*
rm -rf ${TARGET_DIR}/package/mtk/applications/mapd/patches/*
rm -rf ${TARGET_DIR}/package/mtk/applications/wappd/patches/*
rm -rf ${TARGET_DIR}/package/mtk/applications/datconf/patches/*

mkdir ${TARGET_DIR}/dl/
cp -rf dl/*	${TARGET_DIR}/dl/
rm -rf ${TARGET_DIR}/autobuild/airoha/an7563-mt7992-EXS27/openwrt_patches-21.02/*
rm -rf ${TARGET_DIR}/feeds/airoha/openwrt_patches-21.02/*
rm -rf ${TARGET_DIR}/autobuild/openwrt_patches-21.02/*

#The following operation is only applicable to GPL compilation error files
#MTKHNAT_DIR=$(find -name "mtkhnat.ko" | grep "build_dir/target-aarch64_cortex-a53_musl/root-mediatek/lib/modules/5.4.238/mtkhnat.ko")
D2_DIR=$(find -type d -name "d2" | grep "package/tw/applications/d2")
PUSHNTF_DIR=$(find -type d -name "pushntf" | grep "package/tw/applications/pushntf")
UPFWU_DIR=$(find -type d -name "upfwu" | grep "package/tw/applications/upfwu")
BST_DIR=$(find -type d -name "bst" | grep "package/tw/applications/bst")
RAE_DIR=$(find -name "install" | grep "package/tw/applications/raepre")

#mkdir -p ${TARGET_DIR}/${BUILD_DIR}/root-mediatek/lib/modules/5.4.238/
#cp -rf $MTKHNAT_DIR  ${TARGET_DIR}/${BUILD_DIR}/root-mediatek/lib/modules/5.4.238/
cp -rf $D2_DIR/files/*  ${TARGET_DIR}/package/tw/applications/d2/files/
cp -rf $PUSHNTF_DIR/files/*  ${TARGET_DIR}/package/tw/applications/pushntf/files/
cp -rf $UPFWU_DIR/files/*  ${TARGET_DIR}/package/tw/applications/upfwu/files/
cp -rf $BST_DIR/files/*  ${TARGET_DIR}/package/tw/applications/bst/files/
cp -rf $RAE_DIR  ${TARGET_DIR}/package/tw/applications/raepre/src/