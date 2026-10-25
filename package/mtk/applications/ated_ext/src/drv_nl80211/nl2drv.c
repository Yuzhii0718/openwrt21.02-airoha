#include "precomp.h"

/* Functions */
static void *driver_nl80211_init_genl(struct DRI_IF *fd, char *driver_ifname)
{
	fd->drv_nl80211_data = malloc(sizeof(struct driver_nl80211_data));
	if (!fd->drv_nl80211_data) {
		ate_printf(MSG_ERROR, "No available memory for drv_nl80211_data\n");
		goto err1;
	}

	ate_printf(MSG_DEBUG, "Initialize mediatek nl80211 interface start\n");

	if (unl_genl_init(&fd->drv_nl80211_data->nl_handle, NL80211_GENL_NAME) < 0) {
		ate_printf(MSG_ERROR, "failed to connect to nl80211 handle\n");
		goto err2;
	}

	ate_printf(MSG_DEBUG, "Initialize mediatek nl80211 interface done\n");

	return (void *)fd->drv_nl80211_data;
err2:
	free(fd->drv_nl80211_data);
err1:
	return NULL;
}

int driver_nl80211_init_if_ioctl(struct DRI_IF *fd, char *driver_ifname)
{
	struct ifreq ifr;

	fd->send = &driver_nl80211_send;
	fd->close = &driver_nl80211_exit;
	fd->sock_ioctl = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd->sock_ioctl < 0) {
		ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
		return 0;
	}

	os_memset(fd->ifname, 0, IFNAMSIZ);
	os_memset(&ifr, 0, sizeof(struct ifreq));
	os_memcpy(fd->ifname, driver_ifname, strnlen(driver_ifname, IFNAMSIZ));
	/* Get MAC Address */
	ifr.ifr_addr.sa_family = AF_INET;
	os_memcpy(ifr.ifr_name , driver_ifname , strnlen(driver_ifname, IFNAMSIZ));

	if (ioctl(fd->sock_ioctl, SIOCGIFHWADDR, &ifr) != 0) {
		perror("ioctl(SIOCGIFHWADDR)(sock_ioctl)");
	}

	os_memcpy(fd->mac, ifr.ifr_hwaddr.sa_data, 6);

	driver_nl80211_init_genl(fd, driver_ifname);

	return fd->sock_ioctl;
}

static int driver_nl80211_get_msg_cb(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *sub_tb[MTK_NL80211_VENDOR_ATTR_HQA_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct driver_nl80211_data *drv_data = (struct driver_nl80211_data *)arg;
	char *data;
	UINT32 data_len = 0;

	if (gnlh->cmd != NL80211_CMD_VENDOR) {
		ate_printf(MSG_ERROR, "%s: not mtk vendor msg\n", __func__);
		return NL_SKIP;
	}

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		ate_printf(MSG_ERROR, "%s:NL80211_ATTR_VENDOR_DATA missing\n", __func__);
		return NL_SKIP;
	}

	if (nla_parse_nested(sub_tb, MTK_NL80211_VENDOR_ATTR_HQA_MAX, tb[NL80211_ATTR_VENDOR_DATA], NULL)) {
		ate_printf(MSG_ERROR, "%s: failed to parse nested attributes\n", __func__);
		return NL_SKIP;
	}

	if (sub_tb[MTK_NL80211_VENDOR_ATTR_HQA]) {
		data = nla_data(sub_tb[MTK_NL80211_VENDOR_ATTR_HQA]);
		data_len = nla_len(sub_tb[MTK_NL80211_VENDOR_ATTR_HQA]);

		ate_printf(MSG_DEBUG, "%s:get MTK_NL80211_VENDOR_ATTR_HQA: data_len:%u\n", __func__, data_len);

		if (drv_data->get_data) {
			os_memcpy(drv_data->get_data, data, data_len);
			drv_data->get_data_len = data_len;
		}
	}

	ate_printf(MSG_DEBUG, "%s(%d)\n", __func__, __LINE__);

	return NL_SKIP;
}

static int driver_nl80211_send_msg(struct driver_nl80211_data *drv_data, const char *ifname, unsigned short cmd_id,
				  unsigned short sub_cmd_id, char *pkt, size_t len)
{
	struct nl_msg *msg;
	void *nl_data;
	int ret = -1;

	ate_printf(MSG_DEBUG, "%s(%d) ifname:%s\n", __func__, __LINE__, ifname);

	msg = unl_genl_msg(&drv_data->nl_handle, NL80211_CMD_VENDOR, FALSE);
	if (!msg) {
		ate_printf(MSG_ERROR, "unl_genl_msg fail!!!\n");
		return ret;
	}

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(ifname)) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, MTK_NL80211_VENDOR_ID) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD, cmd_id)) {
		ate_printf(MSG_ERROR, "nla_put_u32 fail!!!\n");
		goto out;
	}

	nl_data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
	if (!nl_data)
		goto out;

	if (nla_put(msg, sub_cmd_id, len, pkt))
		goto out;

	/* resp data */
	drv_data->get_data = pkt;
	drv_data->get_data_len = len;

	nla_nest_end(msg, nl_data);

	ret = unl_genl_request(&drv_data->nl_handle, msg, driver_nl80211_get_msg_cb, drv_data);
	if (ret)
		ate_printf(MSG_DEBUG, "nl80211 call fail: %s\n", strerror(-ret));

	return ret;
out:
	nlmsg_free(msg);
	return ret;
}


int driver_nl80211_send(struct DRI_IF *fd, unsigned char *pkt, int size)
{
	ate_printf(MSG_DEBUG, "%s(%d) size:%d\n", __func__, __LINE__, size + RA_CFG_HLEN);

	return driver_nl80211_send_msg(fd->drv_nl80211_data, fd->ifname, 
		MTK_NL80211_VENDOR_SUBCMD_HQA, MTK_NL80211_VENDOR_ATTR_HQA,
		(char *)pkt, size + RA_CFG_HLEN);
}


static int driver_nl80211_close_ioctl(struct DRI_IF *fd)
{
	int ret;

	ate_printf(MSG_INFO, "Close IOCTL Socket\n");
	ret = close(fd->sock_ioctl);

	return ret;
}

int driver_nl80211_exit(struct DRI_IF *fd)
{
	struct driver_nl80211_data *drv_nl80211_data = fd->drv_nl80211_data;

	ate_printf(MSG_DEBUG, "%s\n", __func__);

	unl_free(&drv_nl80211_data->nl_handle);
	free(drv_nl80211_data);
	drv_nl80211_data = NULL;

	driver_nl80211_close_ioctl(fd);

	return 0;
}


