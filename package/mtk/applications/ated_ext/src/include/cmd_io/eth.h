#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>	/*Get MAC Address from kernel*/
#include <arpa/inet.h>

int init_eth(struct HOST_IF *fd, char *bridge_ifname);
int rev_cmd_eth(unsigned char *buf, int size, BOOLEAN is_accept);
void drv_add_addr(char *addr);
int find_mac_index(char *addr);
int rsp2host_eth(unsigned char *data, int size);
int close_eth(void);

extern unsigned short tcp_port;
extern unsigned short udp_port;
extern int cli_fd;
int init_eth_tcp(struct HOST_IF *fd, char *bridge_ifname);
int rev_cmd_eth_tcp(unsigned char *buf, int size, BOOLEAN is_accept);
int rsp2host_eth_tcp(unsigned char *data, int size);
int close_eth_tcp(void);
int close_eth_tcp_init_sock(void);
int init_eth_udp(struct HOST_IF *fd, char *bridge_ifname);
int rev_cmd_eth_udp(unsigned char *buf, int size, BOOLEAN is_accept);
int rsp2host_eth_udp(unsigned char *data, int size);
int close_eth_udp(void);
