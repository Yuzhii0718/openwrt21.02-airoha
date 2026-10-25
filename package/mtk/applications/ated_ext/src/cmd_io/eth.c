#include "precomp.h"

#define NUM_DRIVER_IF 2

int sock_eth = -1;
unsigned char packet[PKT_BUF_SIZE];
static const char broadcast_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static char drv_addr[NUM_DRIVER_IF][6];
static char my_eth_addr[6];
struct HOST_IF *hfd;
static int if_index;
static int addr_num = 0;

int sock_eth_tcp = -1;
int sock_eth_udp = -1;
struct sockaddr_in client_addr;
struct sockaddr    udp_client_addr;
unsigned short tcp_port = TCP_PORT;
unsigned short udp_port = UDP_PORT;
int cli_fd = -1;


int init_eth(struct HOST_IF *fd, char *bridge_ifname)
{
	struct ifreq ifr;
	struct sockaddr_ll addr;
	int s, i = 0;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return -1;
	}

	if ((sock_eth = socket(PF_PACKET, SOCK_RAW, cpu2be16/*htons*/(ETH_P_RACFG))) < 0) {
		perror("socket");
		close(s);
		return -1;
	}

	os_memset(&ifr, 0, sizeof(ifr));
#ifdef CONFIG_LAN_WAN_SUPPORT
	os_memcpy(ifr.ifr_name, "eth2.1" , 7);
#else
	os_memcpy(ifr.ifr_name, bridge_ifname , strnlen(bridge_ifname, IFNAMSIZ));
#endif

	if (ioctl(sock_eth, SIOCGIFINDEX, &ifr) != 0) {
		perror("ioctl(SIOCGIFINDEX)(eth_sock)");
		goto close;
	}

	os_memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifr.ifr_ifindex;
	if_index = ifr.ifr_ifindex;

	if (bind(sock_eth, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		goto close;
	}

	os_memset(&ifr.ifr_hwaddr.sa_data, 0, ETH_ALEN);

	/* Get the eth_addr from kernel and put it into ifreq structure */
	if (ioctl(sock_eth, SIOCGIFHWADDR, &ifr) != 0) {
		perror("ioctl(SIOCGIFHWADDR)(eth_sock)");
		goto close;
	}

	if (fd->need_set_mac) {
		os_memcpy(&ifr.ifr_hwaddr.sa_data, &fd->ownmac, ETH_ALEN);
		ate_printf(MSG_INFO, "Set Own Mac: %02x:%02x:%02x:%02x:%02x:%02x\n", (unsigned char) ifr.ifr_hwaddr.sa_data[0], (unsigned char) ifr.ifr_hwaddr.sa_data[1], (unsigned char) ifr.ifr_hwaddr.sa_data[2], (unsigned char) ifr.ifr_hwaddr.sa_data[3], (unsigned char) ifr.ifr_hwaddr.sa_data[4], (unsigned char) ifr.ifr_hwaddr.sa_data[5]);

		if (ioctl(s, SIOCSIFHWADDR, &ifr) != 0) {
			perror("ioctl(SIOCSIFHWADDR)(eth_sock)");
			goto close;
		}

		ifr.ifr_flags |= IFF_PROMISC;

		if (ioctl(s, SIOCSIFFLAGS, &ifr) != 0) {
			perror("ioctl for IFF_PROMISC failed.");
			goto close;
		}
	}
	else {
		os_memcpy(&fd->ownmac, &ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	}
	for (;i < NUM_DRIVER_IF; i++){
		os_memset(drv_addr[i], 0, ETH_ALEN);
	}
	addr_num = 0;
	os_memcpy(my_eth_addr, ifr.ifr_hwaddr.sa_data, 6);
	ate_printf(MSG_INFO, "Open Ethernet socket success\n");
	fd->rev_cmd = &rev_cmd_eth;
	fd->rsp2host = &rsp2host_eth;
	fd->close = &close_eth;
	fd->hlen = ETH_HLEN;
	hfd = fd;
	os_memset(hfd->da, 0, ETH_ALEN);
	close(s);
	return sock_eth;
close:
	close(s);
	close(sock_eth);
	return (-1);
}

int find_mac_index(char *addr)
{
	int i = 0;
	for (;i < NUM_DRIVER_IF; i++){
		if (memcmp(addr, drv_addr[i], 6) == 0){
			return i;
		}
	}
	return -1;
}

void drv_add_addr(char *addr)
{
		if (addr_num < NUM_DRIVER_IF && find_mac_index(addr) == -1){
			os_memcpy(drv_addr[addr_num], addr, ETH_ALEN);
			addr_num++;
		}
}

int rev_cmd_eth(unsigned char *buf, int size, BOOLEAN is_accept)
{
	int recv_len = recvfrom(sock_eth, packet, PKT_BUF_SIZE, 0, NULL, NULL);
	struct ethhdr *ehdr = (struct ethhdr *)packet;

	if (recv_len >= (ETH_HLEN + RA_CFG_HLEN)) {
		if ((ehdr->h_proto == cpu2be16/*htons*/(ETH_P_RACFG)) &&
			((strncmp(my_eth_addr, (char *)ehdr->h_dest, 6) == 0) ||
			 (strncmp(broadcast_addr, (char *)ehdr->h_dest, 6) == 0) ||
			 (find_mac_index((char *)ehdr->h_dest) >= 0))) {
			os_memcpy(buf, packet, recv_len);
			return recv_len - 14;
		}

		goto err;
	}

err:
	ate_printf(MSG_ERROR, "packet len is too short!\n");
	return 0;
}

int rsp2host_eth(unsigned char *data, int size)
{
	struct ethhdr *ehdr;
	struct sockaddr_ll socket_address;
	int result = 0;
	int length = 0;
	unsigned char da[ETH_ALEN];
	unsigned char sa[ETH_ALEN];
	struct racfghdr *p_tmp = (struct racfghdr *)(data + ETH_HLEN);
	UINT32 magic_no = be2cpu32(p_tmp->magic_no);

	os_memset(da, 0, ETH_ALEN);
	os_memset(sa, 0, ETH_ALEN);
	os_memcpy(packet, data, size);
	/* Respond to QA by broadcast frame */
	ehdr = (struct ethhdr *)&packet[0];
	os_memcpy(da, ehdr->h_source, ETH_ALEN);
	os_memcpy(sa, ehdr->h_dest, ETH_ALEN);

	if (hfd->unicast)
		os_memcpy(packet, da, ETH_ALEN);
	else
		os_memcpy(&packet[0], broadcast_addr, 6);

	if (magic_no == RACFG_MAGIC_NO && hfd->find_mac_index(sa) >= 0)
		os_memcpy(ehdr->h_source, sa, 6);
	else
		os_memcpy(ehdr->h_source, my_eth_addr, 6);
	ehdr->h_proto = cpu2be16/*htons*/(ETH_P_RACFG);
	/* Determine the length to send and send Ack */
	length = size;

	if (length < 60)
		length = 60;
	else if (length > 1514) {
		ate_printf(MSG_ERROR, "response ethernet length is too long\n");
		return -1;
	}

	socket_address.sll_family = PF_PACKET;
	socket_address.sll_protocol = cpu2be16/*htons*/(ETH_P_RACFG);
	socket_address.sll_ifindex = if_index;
	socket_address.sll_pkttype = PACKET_BROADCAST;
	socket_address.sll_hatype = ARPHRD_ETHER;
	socket_address.sll_halen = ETH_ALEN;
	bzero(&socket_address.sll_addr[0], 8);
	os_memcpy(&socket_address.sll_addr[0], broadcast_addr, 6);
	result = sendto(sock_eth, &packet[0], length, 0, (struct sockaddr *)&socket_address, sizeof(socket_address));

	return result;
}

int close_eth(void)
{
	ate_printf(MSG_DEBUG, "Ethernet Socket Is Closed\n");
	if (sock_eth) {
		close(sock_eth);
		sock_eth = -1;
	}

	return 0;
}

/* TCP handle section */
int sockpairfd[2] = { -1, -1};

int eth_open_socketpair(void)
{
	/*  create the socketpair */
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockpairfd) == -1)
		printf("create socketpair error: %d\n", errno);

	return -1;
}

int eth_close_socketpair(void)
{
	close(sockpairfd[0]);
	sockpairfd[0] = -1;
	close(sockpairfd[1]);
	sockpairfd[1] = -1;
	return 0;
}

/* child recv sock fd from parent */
int eth_recv_cli_sock_fd(int fd, void *data, size_t len, int *recv_fd)
{
	struct msghdr msghdr_recv;
	struct iovec  iov[1];
	ssize_t n = 0;
	union {
		struct cmsghdr	cm;
		char   ctl[CMSG_SPACE(sizeof(int))];
	} ctl_un;
	struct cmsghdr *pCmsghdr = NULL;

	bzero(&msghdr_recv, sizeof(struct msghdr));
	msghdr_recv.msg_control = ctl_un.ctl;
	msghdr_recv.msg_controllen = sizeof(ctl_un.ctl);

	/* these infos are nosignification */
	msghdr_recv.msg_name = NULL;
	msghdr_recv.msg_namelen = 0;

	iov[0].iov_base = data;
	iov[0].iov_len = len;

	msghdr_recv.msg_iov = iov;
	msghdr_recv.msg_iovlen = 1;

	n = recvmsg(fd, &msghdr_recv, 0);
	if (n < 0) {
		/* the msg is recv by msghdr_recv */
		printf("recv error : %d\n", errno);
	}
	ate_printf(MSG_INFO, "%s(%d) recvmsg byte:%ld\n", __func__, __LINE__, n);

	/* we not use 'for' just because only one test_data */
	pCmsghdr = CMSG_FIRSTHDR(&msghdr_recv);
	if (pCmsghdr != NULL && pCmsghdr->cmsg_len == CMSG_LEN(sizeof(int))) {
		if (pCmsghdr->cmsg_level != SOL_SOCKET)
			printf("Ctl level should be SOL_SOCKET\n");

		if (pCmsghdr->cmsg_type != SCM_RIGHTS)
			printf("Ctl type should be SCM_RIGHTS\n");

		/* get the data : the cli sock des */
		*recv_fd = *((int *)CMSG_DATA(pCmsghdr));
	} else {
		*recv_fd = -1;
	}

	return n;
}

/* parent send sock fd to child */
int eth_send_cli_sock_fd(int fd_send_to, void *data, size_t len, int file_fd)
{
	struct msghdr msghdr_send;
	struct iovec  iov[1];
	union {
		struct cmsghdr	cm;
		char   ctl[CMSG_SPACE(sizeof(int))];
	} ctl_un;
	struct cmsghdr *pCmsghdr = NULL;
	int n = 0;

	bzero(&msghdr_send, sizeof(struct msghdr));
	msghdr_send.msg_control = ctl_un.ctl;
	msghdr_send.msg_controllen = sizeof(ctl_un.ctl);

	pCmsghdr = CMSG_FIRSTHDR(&msghdr_send);
	pCmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
	pCmsghdr->cmsg_level = SOL_SOCKET;
	pCmsghdr->cmsg_type = SCM_RIGHTS;
	*((int *)CMSG_DATA(pCmsghdr)) = file_fd;

	msghdr_send.msg_name = NULL;
	msghdr_send.msg_namelen = 0;

	iov[0].iov_base = data;
	iov[0].iov_len = len;

	msghdr_send.msg_iov = iov;
	msghdr_send.msg_iovlen = 1;

	n = sendmsg(fd_send_to, &msghdr_send, 0);
	return n;
}

int init_eth_tcp(struct HOST_IF *fd, char *bridge_ifname)
{
	struct sockaddr_in dut_addr;
	int reuse = 1, status;

	/* create a socket */
	sock_eth_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_eth_tcp == -1) {
		perror("create stream socket fail");
		return -1;
	}

	/* for "Address already in use" error message */
	if (setsockopt(sock_eth_tcp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
		perror("Setsockopt (SO_REUSEADDR) failed");
		return -1;
	}

	os_memset(&dut_addr, 0, sizeof(dut_addr));
	dut_addr.sin_family = PF_INET;
	/* dut_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
	dut_addr.sin_addr.s_addr = INADDR_ANY;
	dut_addr.sin_port = htons(tcp_port);

	status = bind(sock_eth_tcp, (struct sockaddr *)&dut_addr, sizeof(dut_addr));
	if (status == -1) {
		perror("binding error");
		return -1;
	}
	ate_printf(MSG_INFO, "tcp serv start at: %s: %d\n", inet_ntoa(dut_addr.sin_addr), tcp_port);

	status = listen(sock_eth_tcp, 5);
	if (status == -1) {
		perror("Listening error");
		return -1;
	}
	printf("wait for tcp connection...\n");

	ate_printf(MSG_INFO, "Open Ethernet tcp socket success\n");
	fd->rev_cmd = &rev_cmd_eth_tcp;
	fd->rsp2host = &rsp2host_eth_tcp;
	fd->close = &close_eth_tcp;
	fd->hlen = 0;

	eth_open_socketpair();

	return sock_eth_tcp;
}


int rev_cmd_eth_tcp(unsigned char *buf, int size, BOOLEAN is_accept)
{
	socklen_t addrlen = 0;
	int byte;

	addrlen = sizeof(client_addr);

	if (is_accept)
		cli_fd = accept(sock_eth_tcp, (struct sockaddr *)&client_addr, &addrlen);

	while (cli_fd > 0) {
		int nbytes = recv(cli_fd, packet, sizeof(packet), 0);

		if (nbytes <= 0) {
			perror("tcp recv:");
			close(cli_fd);
			cli_fd = -1;
			printf("nbytes: %d\n", nbytes);
			break;
		}
		os_memcpy(buf, packet, nbytes);
		ate_printf(MSG_INFO, "%s: nbytes:%d pid:%d, cli_fd:%d\n", __func__, nbytes, getpid(), cli_fd);

		byte = eth_send_cli_sock_fd(sockpairfd[1], "", 1, cli_fd);
		ate_printf(MSG_INFO, "%s: send byte:%d\n", __func__, byte);
		return nbytes;
	}

	ate_printf(MSG_ERROR, "packet len is too short!\n");
	return 0;
}

int rsp2host_eth_tcp(unsigned char *data, int size)
{
	int len = 0;
	int new_cli_fd = 0;
	char c = "";

	os_memcpy(packet, data, size);

	/* get cli fd for parent */
	eth_recv_cli_sock_fd(sockpairfd[0], &c, 1, &new_cli_fd);
	if (new_cli_fd < 0)
		perror("eth_recv_cli_sock_fd");

	/* Respond to QA by broadcast frame */
	len = send(new_cli_fd, packet, size, 0);
	if (len < 0)
		perror("rsp2host_eth_tcp send");

	ate_printf(MSG_INFO, "%s %d, send_len:%d, pid:%d, new_cli_fd:%d\n",
		__func__, __LINE__, len, getpid(), new_cli_fd);

	close(new_cli_fd);

	return len;
}

int close_eth_tcp(void)
{
	ate_printf(MSG_DEBUG, "Ethernet socket TCP is closed\n");
	if (sock_eth_tcp) {
		close(sock_eth_tcp);
		sock_eth_tcp = -1;
	}

	eth_close_socketpair();

	return 0;
}

int close_eth_tcp_init_sock(void)
{
	if (sock_eth_tcp) {
		close(sock_eth_tcp);
		sock_eth_tcp = -1;
	}

	return 0;
}

/* UDP handle section */
int init_eth_udp(struct HOST_IF *fd, char *bridge_ifname)
{
	struct sockaddr_in dut_addr;
	int status = 0;

	/* create a socket */
	sock_eth_udp  = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_eth_udp == -1) {
		perror("create stream socket fail");
		return -1;
	}

	memset((char *) &client_addr, 0, sizeof(client_addr));
	memset((char *) &dut_addr, 0, sizeof(dut_addr));

	os_memset(&dut_addr, 0, sizeof(dut_addr));
	dut_addr.sin_family = AF_INET;
	/* dut_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
	dut_addr.sin_addr.s_addr = INADDR_ANY;
	dut_addr.sin_port = htons(udp_port);

	status = bind(sock_eth_udp, (struct sockaddr *)&dut_addr, sizeof(dut_addr));
	if (status == -1) {
		perror("binding error");
		return -1;
	}
	printf("server start at: %s : port %d\n", inet_ntoa(dut_addr.sin_addr), udp_port);

	printf("wait for recv...\n");

	ate_printf(MSG_INFO, "Open Ethernet tcp socket success\n");
	fd->rev_cmd = &rev_cmd_eth_udp;
	fd->rsp2host = &rsp2host_eth_udp;
	fd->close = &close_eth_udp;
	fd->hlen = 0;

	/* eth_open_socketpair(); */

	return sock_eth_udp;
}

int rev_cmd_eth_udp(unsigned char *buf, int size, BOOLEAN is_accept)
{
	socklen_t addrlen = 0;
	int recv_len;

	addrlen = sizeof(client_addr);

	recv_len = recvfrom(sock_eth_udp, packet, PKT_BUF_SIZE, 0, &udp_client_addr, &addrlen);
	if (recv_len < 0) {
		perror("ERROR in recvfrom");
		goto err;
	}

	os_memcpy(buf, packet, recv_len);

	return recv_len;

err:
	ate_printf(MSG_ERROR, "packet len is too short!\n");
	return 0;
}


int rsp2host_eth_udp(unsigned char *data, int size)
{
	int result = 0;
	int addrlen = sizeof(udp_client_addr);

	os_memcpy(packet, data, size);

	result = sendto(sock_eth_udp, packet, size, 0, (struct sockaddr *)&udp_client_addr, addrlen);
	if (result < 0)
		perror("send");

	return result;
}

int close_eth_udp(void)
{
	ate_printf(MSG_DEBUG, "Ethernet socket TCP is closed\n");
	if (sock_eth_udp) {
		close(sock_eth_udp);
		sock_eth_udp = -1;
	}

	/* eth_close_socketpair(); */

	return 0;
}
