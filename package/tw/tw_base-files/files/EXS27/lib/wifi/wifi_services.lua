--This file is created for check some deamons

local mtkdat = require("mtkdat")
local nixio = require("nixio")
local mtkwifi = require("mtkwifi")

-- tw add
function guestNetActive(vif, allowed)
    os.execute("ebtables -D INPUT -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 67:68 -j ACCEPT 1>/dev/null 2>&1")
    os.execute("ebtables -D INPUT -p ARP -i "..vif.." -j ACCEPT 1>/dev/null 2>&1")
    os.execute("ebtables -D INPUT -i "..vif.." -j DROP 1>/dev/null 2>&1")

    if allowed == "0" then
        os.execute("ebtables -A INPUT -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 67:68 -j ACCEPT")
        os.execute("ebtables -A INPUT -p ARP -i "..vif.." -j ACCEPT")
        -- os.execute("ebtables -A INPUT -i "..vif.." -j DROP")
    end

    os.execute("ebtables -D FORWARD -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 53 -j ACCEPT 1>/dev/null 2>&1")
    os.execute("ebtables -D FORWARD -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 67:68 -j ACCEPT 1>/dev/null 2>&1")

    if allowed == "0" then
        os.execute("ebtables -A FORWARD -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 53 -j ACCEPT")
        os.execute("ebtables -A FORWARD -p IPv4 -i "..vif.." --ip-proto 17 --ip-dport 67:68 -j ACCEPT")
    end

    local fd = io.open("/tmp/br-lan-ipaddr"..vif, "r")
    if fd ~= nil then
        local oldIp = fd:read("*l")
        fd:close()
        if oldIp ~= nil then
            os.execute("ebtables -D FORWARD -p IPv4 -i "..vif.." --ip-dst "..oldIp.." -j DROP 1>/dev/null 2>&1")
            os.execute("ebtables -D INPUT -p IPv4 -i "..vif.." --ip-dst "..oldIp.." -j DROP 1>/dev/null 2>&1")
        end
    end

    local cmdfd = io.popen("ifconfig br-lan | awk -F '[ :]+' '/inet addr/ {print $4}'")
    local curIp = cmdfd:read("*l")
    cmdfd:close()

    local cmdfd = io.popen("ifconfig br-lan | awk -F '[ :]+' '/inet addr/ {print $8}'")
    local curIpMask = cmdfd:read("*l")
    cmdfd:close()

    if allowed == "0"  and curIp ~= nil and curIpMask ~= nil then
        os.execute("ebtables -A FORWARD -p IPv4 -i "..vif.." --ip-dst "..curIp.."/"..curIpMask.." -j DROP")
        os.execute("ebtables -A INPUT -p IPv4 -i "..vif.." --ip-dst "..curIp.."/"..curIpMask.." -j DROP")
    end

    if curIp ~= nil and  curIpMask ~= nil then
        local fd = io.open("/tmp/br-lan-ipaddr"..vif, "w")
        if nil ~= fd then
            fd:write(curIp.."/"..curIpMask)
            fd:close()
        end
    else
        os.remove("/tmp/br-lan-ipaddr"..vif)
    end
end

function guestNetwork()
    local if2g="ra1"
    local if5g="rai1"
    local guest2g = mtkwifi.__trim(mtkwifi.read_pipe("uci get wireless.ra1.allowLocalNetwork"))
    local guest5g = mtkwifi.__trim(mtkwifi.read_pipe("uci get wireless.rai1.allowLocalNetwork"))

    if mtkwifi.exists("/sys/class/net/"..if2g) then
        guestNetActive(if2g, guest2g)
    end

    if mtkwifi.exists("/sys/class/net/"..if5g) then
        guestNetActive(if5g, guest5g)
    end
end

function setNtgrVie()
    local if2g="ra0"
    local if5g="rai0"
	local conf_mode=mtkwifi.__trim(mtkwifi.read_pipe("uci -q get wireless.2g_5g.conf_mode"))
    local ap2gRadio=mtkwifi.__trim(mtkwifi.read_pipe("uci -q get wireless.ra0.disabled"))
    local ap5gRadio=mtkwifi.__trim(mtkwifi.read_pipe("uci -q get wireless.rai0.disabled"))

	os.execute("rm /tmp/setntgrvie 1>/dev/null 2>&1")

	-- ap mode
	if conf_mode == '0' then
		if mtkwifi.exists("/sys/class/net/"..if2g) and ap2gRadio == '0' then
			mtkwifi.set_netgear_vie(if2g, 0)
		end

		if mtkwifi.exists("/sys/class/net/"..if5g) and ap5gRadio == '0' then
			mtkwifi.set_netgear_vie(if5g, 0)
		end
	else
		if mtkwifi.exists("/sys/class/net/"..if2g) and ap2gRadio == '0' then
			mtkwifi.set_netgear_vie(if2g, 1)
		end

		if mtkwifi.exists("/sys/class/net/"..if5g) and ap5gRadio == '0' then
			mtkwifi.set_netgear_vie(if5g, 1)
		end
	end

	os.execute("echo 1 > /tmp/setntgrvie")

end

function connetRootAp(dev)
    local conf_mode=mtkdat.__trim(mtkdat.read_pipe("uci -q get wireless.2g_5g.conf_mode"))

    if conf_mode ~= '0' then
        local isRootApExist_2g=mtkdat.__trim(mtkdat.read_pipe("uci -q get wireless.apcli0.disabled"))
        local isRootApExist_5g=mtkdat.__trim(mtkdat.read_pipe("uci -q get wireless.apclii0.disabled"))
        local devname_2g=mtkdat.__trim(mtkdat.read_pipe("uci -q get wireless.apcli0.device"))
        local devname_5g=mtkdat.__trim(mtkdat.read_pipe("uci -q get wireless.apclii0.device"))

        if devname_2g then devname_2g = devname_2g:gsub("%_", ".") end
        if devname_5g then devname_5g = devname_5g:gsub("%_", ".") end

        if isRootApExist_2g == '0' and dev == "2g" then
            for _,vif in ipairs(string.split(mtkdat.read_pipe("ls /sys/class/net"), "\n"))
            do
                if string.match(vif, "apcli%d") then
                    mtkdat.apcli_connect(devname_2g, vif);
                end
            end
        end

        if isRootApExist_5g == '0' and dev == "5g" then
            for _,vif in ipairs(string.split(mtkdat.read_pipe("ls /sys/class/net"), "\n"))
            do
                if string.match(vif, "apclii%d") then
                    mtkdat.apcli_connect(devname_5g, vif);
                end
            end
        end
    end
end

function set_otp()
    local if2g="ra0"
    local if5g="rai0"    
    local ap2gRadio=mtkwifi.__trim(mtkwifi.read_pipe("uci -q get wireless.ra0.disabled"))
    local ap5gRadio=mtkwifi.__trim(mtkwifi.read_pipe("uci -q get wireless.rai0.disabled"))
    nixio.syslog("debug","[ap2gRadio="..ap2gRadio.."], [ap5gRadio="..ap5gRadio.."]")

    if mtkwifi.exists("/sys/class/net/"..if2g) and ap2gRadio == '0' then
        if mtkwifi.exists("/tmp/logdbg") then
            os.execute("mwctl ra0 set Debug=3")
        else
            os.execute("mwctl ra0 set Debug=1")
        end
        os.execute("mwctl ra0 set thermal_protect_duty_cfg=0:0:100")
        os.execute("mwctl ra0 set thermal_protect_duty_cfg=0:1:85")
        os.execute("mwctl ra0 set thermal_protect_duty_cfg=0:2:70")
        os.execute("mwctl ra0 set thermal_protect_duty_cfg=0:3:60")
        os.execute("mwctl ra0 set thermal_protect_enable=0:1:1:124:118:0005")
    end

    if mtkwifi.exists("/sys/class/net/"..if5g) and ap5gRadio == '0' then
        if mtkwifi.exists("/tmp/logdbg") then
            os.execute("mwctl rai0 set Debug=3")
        else
            os.execute("mwctl rai0 set Debug=1")
        end
        os.execute("mwctl rai0 set thermal_protect_duty_cfg=1:0:100")
        os.execute("mwctl rai0 set thermal_protect_duty_cfg=1:1:85")
        os.execute("mwctl rai0 set thermal_protect_duty_cfg=1:2:70")
        os.execute("mwctl rai0 set thermal_protect_duty_cfg=1:3:60")
        os.execute("mwctl rai0 set thermal_protect_enable=1:1:1:124:118:0005")
    end
end

--tw add endif

-- wifi service that require to start after wifi up
function wifi_service_misc()
    local mapd_default = mtkdat.load_profile("/etc/map/mapd_default.cfg")
    local mapd_user = mtkdat.load_profile("/etc/map/mapd_user.cfg")
    local first_card_cfgs = mtkdat.load_profile(mtkdat.detect_first_card())
-- tw add
    os.execute("echo [wifi] ENTER wifi_service_misc >> /tmp/wifiLuaDebug.log")

    setNtgrVie()

    connetRootAp("2g")
    connetRootAp("5g")

    guestNetwork()

    os.execute("/lib/functions/wpsled.sh")

    if mtkwifi.exists("/tmp/mtk/wifi/normal_reload") then
        os.execute("rm /tmp/mtk/wifi/normal_reload")
    end

    os.execute("iptables -t mangle -D OUTPUT -p udp --dport 123 -j DSCP --set-dscp 0")
    os.execute("iptables -t mangle -A OUTPUT -p udp --dport 123 -j DSCP --set-dscp 0")
--tw add endif
    if mapd_default.mode then
        local eth_mode = mapd_default.mode
        local device_role = mapd_default.DeviceRole
        if mapd_user.mode then
            eth_mode = mapd_user.mode
        end
        if mapd_user.DeviceRole then
            device_role = mapd_user.DeviceRole
        end
        -- 1.Wapp
        if mtkdat.exist("/usr/bin/wapp_openwrt.sh") then
            os.execute("/etc/init.d/wapp start")
        end
        -- 2.EasyMesh
        if mtkdat.exist("/usr/bin/EasyMesh_openwrt.sh") then
            if first_card_cfgs.MapMode == "1" then
                if (eth_mode == "0" and device_role == "1") or eth_mode == "1" then
                    os.execute("/etc/init.d/easymesh start")
                else
                    os.execute("/etc/init.d/easymesh_bridge start")
                end
            --elseif first_card_cfgs.MapMode ~= "0" then
            else
                os.execute("/etc/init.d/easymesh start")
            end
        end
    else
        -- 1.Wapp
        if mtkdat.exist("/usr/bin/wapp_openwrt.sh") then
            os.execute("/etc/init.d/wapp start")
        end
        -- 2.EasyMesh
        if mtkdat.exist("/usr/bin/EasyMesh_openwrt.sh") then
            os.execute("/etc/init.d/easymesh start")
        end
    end
    -- os.execute("/etc/init.d/fwdd restart") -- should move to EasyMesh_openwrt.sh to make sure it's started on time.
    --set otp
    local factory_mode=mtkwifi.__trim(mtkwifi.read_pipe("envctl factory get factory_mode"))
    nixio.syslog("debug","lua factory_mode:"..factory_mode)
    if factory_mode == "off" then
        nixio.syslog("debug"," wifi_services.lua set otp --------")
        set_otp()
    end
    
    
end

-- wifi service that require to clean up before wifi down
function wifi_service_misc_clean()
    os.execute("rm -rf /tmp/wapp_ctrl")
    os.execute("killall -15 mapd")
    os.execute("killall -15 wapp")
    os.execute("killall -15 p1905_managerd")
    os.execute("killall -15 bs20")
end
