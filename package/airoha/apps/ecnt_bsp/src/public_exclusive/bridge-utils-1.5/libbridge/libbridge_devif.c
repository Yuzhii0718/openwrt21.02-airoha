/*
 * Copyright (C) 2000 Lennert Buytenhek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/fcntl.h>

#include "libbridge_private.h"
#include "libbridge.h"

static FILE *fpopen(const char *dir, const char *name)
{
	char path[SYSFS_PATH_MAX];

	snprintf(path, SYSFS_PATH_MAX, "%s/%s", dir, name);
	return fopen(path, "r");
}

static void fetch_id(const char *dev, const char *name, struct bridge_id *id)
{
	FILE *f = fpopen(dev, name);

	if (!f)
		fprintf(stderr, "%s: %s\n", dev, strerror(errno));
	else {
		fscanf(f, "%2hhx%2hhx.%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
		       &id->prio[0], &id->prio[1],
		       &id->addr[0], &id->addr[1], &id->addr[2],
		       &id->addr[3], &id->addr[4], &id->addr[5]);
		fclose(f);
	}
}

/* Fetch an integer attribute out of sysfs. */
static int fetch_int(const char *dev, const char *name)
{
	FILE *f = fpopen(dev, name);
	int value = -1;

	if (!f) 
		return 0;

	fscanf(f, "%i", &value);
	fclose(f);
	return value;
}

/* Get a time value out of sysfs */
static void fetch_tv(const char *dev, const char *name, 
		    struct timeval *tv)
{
	__jiffies_to_tv(tv, fetch_int(dev, name));
}

/*
 * Convert device name to an index in the list of ports in bridge.
 *
 * Old API does bridge operations as if ports were an array
 * inside bridge structure.
 */
static int get_portno(const char *brname, const char *ifname)
{
	int i;
	int ifindex = if_nametoindex(ifname);
	int ifindices[MAX_PORTS];
	unsigned long args[4] = { BRCTL_GET_PORT_LIST,
				  (unsigned long)ifindices, MAX_PORTS, 0 };
	struct ifreq ifr;

	if (ifindex <= 0)
		goto error;

	memset(ifindices, 0, sizeof(ifindices));
	strncpy(ifr.ifr_name, brname, IFNAMSIZ);
	ifr.ifr_data = (char *) &args;

	if (ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr) < 0) {
		dprintf("get_portno: get ports of %s failed: %s\n", 
			brname, strerror(errno));
		goto error;
	}

	for (i = 0; i < MAX_PORTS; i++) {
		if (ifindices[i] == ifindex)
			return i;
	}

	dprintf("%s is not a in bridge %s\n", ifname, brname);
 error:
	return -1;
}

/* get information via ioctl */
static int old_get_bridge_info(const char *bridge, struct bridge_info *info)
{
	struct ifreq ifr;
	struct __bridge_info i;
	unsigned long args[4] = { BRCTL_GET_BRIDGE_INFO,
				  (unsigned long) &i, 0, 0 };

	memset(info, 0, sizeof(*info));
	strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
	ifr.ifr_data = (char *) &args;

	if (ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr) < 0) {
		dprintf("%s: can't get info %s\n",
			bridge, strerror(errno));
		return errno;
	}

	memcpy(&info->designated_root, &i.designated_root, 8);
	memcpy(&info->bridge_id, &i.bridge_id, 8);
	info->root_path_cost = i.root_path_cost;
	info->root_port = i.root_port;
	info->topology_change = i.topology_change;
	info->topology_change_detected = i.topology_change_detected;
	info->stp_enabled = i.stp_enabled;
	__jiffies_to_tv(&info->max_age, i.max_age);
	__jiffies_to_tv(&info->hello_time, i.hello_time);
	__jiffies_to_tv(&info->forward_delay, i.forward_delay);
	__jiffies_to_tv(&info->bridge_max_age, i.bridge_max_age);
	__jiffies_to_tv(&info->bridge_hello_time, i.bridge_hello_time);
	__jiffies_to_tv(&info->bridge_forward_delay, i.bridge_forward_delay);
	__jiffies_to_tv(&info->ageing_time, i.ageing_time);
	__jiffies_to_tv(&info->hello_timer_value, i.hello_timer_value);
	__jiffies_to_tv(&info->tcn_timer_value, i.tcn_timer_value);
	__jiffies_to_tv(&info->topology_change_timer_value, 
			i.topology_change_timer_value);
	__jiffies_to_tv(&info->gc_timer_value, i.gc_timer_value);

#ifdef TCSUPPORT_IGMP_SNOOPING
        info->igmpsnoop_enabled = i.igmpsnoop_enabled;
#ifdef TCSUPPORT_SNOOPING_SEPERATION
		info->mldsnoop_enabled = i.mldsnoop_enabled;
#endif
        info->igmpsnoop_quickleave = i.igmpsnoop_quickleave;
#ifndef TCSUPPORT_IGMPSNOOPING_ENHANCE
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,18,21)
        info->igmpsnoop_routeportflag = i.igmpsnoop_routeportflag;
#endif
#endif
        info->igmpsnoop_dbg = i.igmpsnoop_dbg;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_IGMP_SET_GROUP)
        info->igmpsnoop_max_group = i.igmpsnoop_max_group;
#endif/*TCSUPPORT_COMPILE*/
        __jiffies_to_tv(&info->igmpsnoop_ageing_time, 
                i.igmpsnoop_ageing_time);
#endif

	return 0;
}

/*
 * Get bridge parameters using either sysfs or old
 * ioctl.
 */
int br_get_bridge_info(const char *bridge, struct bridge_info *info)
{
#ifdef TCSUPPORT_IGMP_SNOOPING
	return old_get_bridge_info(bridge, info);
#else

	DIR *dir;
	char path[SYSFS_PATH_MAX];

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/bridge", bridge);
	dir = opendir(path);
	if (dir == NULL) {
		dprintf("path '%s' is not a directory\n", path);
		goto fallback;
	}

	memset(info, 0, sizeof(*info));
	fetch_id(path, "root_id", &info->designated_root);
	fetch_id(path, "bridge_id", &info->bridge_id);
	info->root_path_cost = fetch_int(path, "root_path_cost");
	fetch_tv(path, "max_age", &info->max_age);
	fetch_tv(path, "hello_time", &info->hello_time);
	fetch_tv(path, "forward_delay", &info->forward_delay);
	fetch_tv(path, "max_age", &info->bridge_max_age);
	fetch_tv(path, "hello_time", &info->bridge_hello_time);
	fetch_tv(path, "forward_delay", &info->bridge_forward_delay);
	fetch_tv(path, "ageing_time", &info->ageing_time);
	fetch_tv(path, "hello_timer", &info->hello_timer_value);
	fetch_tv(path, "tcn_timer", &info->tcn_timer_value);
	fetch_tv(path, "topology_change_timer", 
		 &info->topology_change_timer_value);;
	fetch_tv(path, "gc_timer", &info->gc_timer_value);

	info->root_port = fetch_int(path, "root_port");
	info->stp_enabled = fetch_int(path, "stp_state");
	info->topology_change = fetch_int(path, "topology_change");
	info->topology_change_detected = fetch_int(path, "topology_change_detected");

#ifdef TCSUPPORT_IGMP_SNOOPING
	info->igmpsnoop_enabled = fetch_int(path, "igmpsnoop_state");
	info->igmpsnoop_quickleave = fetch_int(path, "igmpsnoop_quickleave");
#ifndef TCSUPPORT_IGMPSNOOPING_ENHANCE
#ifndef TCSUPPORT_3_18_21_KERNEL
	info->igmpsnoop_routeportflag = fetch_int(path, "igmpsnoop_routeportflag");
#endif
#endif
	info->igmpsnoop_dbg = fetch_int(path, "igmpsnoop_dbg");;

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_IGMP_SET_GROUP)
	info->igmpsnoop_max_group = fetch_int(path, "igmpsnoop_maxgroup");;
#endif/*TCSUPPORT_COMPILE*/

	fetch_tv(path, "igmpsnoop_ageing_time", &info->igmpsnoop_ageing_time);
#endif
    
	closedir(dir);
	return 0;

fallback:
	return old_get_bridge_info(bridge, info);
	
#endif
}

static int old_get_port_info(const char *brname, const char *port,
			     struct port_info *info)
{
	struct __port_info i;
	int index;

	memset(info, 0, sizeof(*info));

	index = get_portno(brname, port);
	if (index < 0)
		return errno;
	
	else {
		struct ifreq ifr;
		unsigned long args[4] = { BRCTL_GET_PORT_INFO,
					   (unsigned long) &i, index, 0 };
	
		strncpy(ifr.ifr_name, brname, IFNAMSIZ);
		ifr.ifr_data = (char *) &args;
		
		if (ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr) < 0) {
			dprintf("old can't get port %s(%d) info %s\n",
				brname, index, strerror(errno));
			return errno;
		}
	}

	info->port_no = index;
	memcpy(&info->designated_root, &i.designated_root, 8);
	memcpy(&info->designated_bridge, &i.designated_bridge, 8);
	info->port_id = i.port_id;
	info->designated_port = i.designated_port;
	info->path_cost = i.path_cost;
	info->designated_cost = i.designated_cost;
	info->state = i.state;
	info->top_change_ack = i.top_change_ack;
	info->config_pending = i.config_pending;
	__jiffies_to_tv(&info->message_age_timer_value, 
			i.message_age_timer_value);
	__jiffies_to_tv(&info->forward_delay_timer_value, 
			i.forward_delay_timer_value);
	__jiffies_to_tv(&info->hold_timer_value, i.hold_timer_value);
	info->hairpin_mode = 0;
	return 0;
}

/*
 * Get information about port on bridge.
 */
int br_get_port_info(const char *brname, const char *port, 
		     struct port_info *info)
{
	DIR *d;
	char path[SYSFS_PATH_MAX];

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/brport", port);
	d = opendir(path);
	if (!d)
		goto fallback;

	memset(info, 0, sizeof(*info));

	fetch_id(path, "designated_root", &info->designated_root);
	fetch_id(path, "designated_bridge", &info->designated_bridge);
	info->port_no = fetch_int(path, "port_no");
	info->port_id = fetch_int(path, "port_id");
	info->designated_port = fetch_int(path, "designated_port");
	info->path_cost = fetch_int(path, "path_cost");
	info->designated_cost = fetch_int(path, "designated_cost");
	info->state = fetch_int(path, "state");
	info->top_change_ack = fetch_int(path, "change_ack");
	info->config_pending = fetch_int(path, "config_pending");
	fetch_tv(path, "message_age_timer", &info->message_age_timer_value);
	fetch_tv(path, "forward_delay_timer", &info->forward_delay_timer_value);
	fetch_tv(path, "hold_timer", &info->hold_timer_value);
	info->hairpin_mode = fetch_int(path, "hairpin_mode");

	closedir(d);

	return 0;
fallback:
	return old_get_port_info(brname, port, info);
}


static int br_set(const char *bridge, const char *name,
		  unsigned long value, unsigned long oldcode)
{
	int ret;
	char path[SYSFS_PATH_MAX];
	FILE *f;

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/%s", bridge, name);

	f = fopen(path, "w");
	if (f) {
		ret = fprintf(f, "%ld\n", value);
		fclose(f);
	} else {
		/* fallback to old ioctl */
		struct ifreq ifr;
		unsigned long args[4] = { oldcode, value, 0, 0 };
		
		strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
		ifr.ifr_data = (char *) &args;
		ret = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
	}

	return ret < 0 ? errno : 0;
}

int br_set_bridge_forward_delay(const char *br, struct timeval *tv)
{
	return br_set(br, "forward_delay", __tv_to_jiffies(tv),
		      BRCTL_SET_BRIDGE_FORWARD_DELAY);
}

int br_set_bridge_hello_time(const char *br, struct timeval *tv)
{
	return br_set(br, "hello_time", __tv_to_jiffies(tv),
		      BRCTL_SET_BRIDGE_HELLO_TIME);
}

int br_set_bridge_max_age(const char *br, struct timeval *tv)
{
	return br_set(br, "max_age", __tv_to_jiffies(tv),
		      BRCTL_SET_BRIDGE_MAX_AGE);
}

int br_set_ageing_time(const char *br, struct timeval *tv)
{
	return br_set(br, "ageing_time", __tv_to_jiffies(tv),
		      BRCTL_SET_AGEING_TIME);
}

int br_set_stp_state(const char *br, int stp_state)
{
	return br_set(br, "stp_state", stp_state, BRCTL_SET_BRIDGE_STP_STATE);
}

int br_set_bridge_priority(const char *br, int bridge_priority)
{
	return br_set(br, "priority", bridge_priority, 
		      BRCTL_SET_BRIDGE_PRIORITY);
}

static int port_set(const char *bridge, const char *ifname, 
		    const char *name, unsigned long value, 
		    unsigned long oldcode)
{
	int ret;
	char path[SYSFS_PATH_MAX];
	FILE *f;

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/brport/%s", ifname, name);
	f = fopen(path, "w");
	if (f) {
		ret = fprintf(f, "%ld\n", value);
		fclose(f);
	} else {
		int index = get_portno(bridge, ifname);

		if (index < 0)
			ret = index;
		else {
			struct ifreq ifr;
			unsigned long args[4] = { oldcode, index, value, 0 };
			
			strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
			ifr.ifr_data = (char *) &args;
			ret = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
		}
	}

	return ret < 0 ? errno : 0;
}

int br_set_port_priority(const char *bridge, const char *port, int priority)
{
	return port_set(bridge, port, "priority", priority, BRCTL_SET_PORT_PRIORITY);
}

int br_set_path_cost(const char *bridge, const char *port, int cost)
{
	return port_set(bridge, port, "path_cost", cost, BRCTL_SET_PATH_COST);
}

int br_set_hairpin_mode(const char *bridge, const char *port, int hairpin_mode)
{
	return port_set(bridge, port, "hairpin_mode", hairpin_mode, 0);
}

static inline void __copy_fdb(struct fdb_entry *ent, 
			      const struct __fdb_entry *f)
{
	memcpy(ent->mac_addr, f->mac_addr, 6);
	ent->port_no = f->port_no;
	ent->is_local = f->is_local;
	__jiffies_to_tv(&ent->ageing_timer_value, f->ageing_timer_value);
}

int br_read_fdb(const char *bridge, struct fdb_entry *fdbs, 
		unsigned long offset, int num)
{
	FILE *f;
	int i, n;
	struct __fdb_entry fe[num];
	char path[SYSFS_PATH_MAX];
	
	/* open /sys/class/net/brXXX/brforward */
	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/brforward", bridge);
	f = fopen(path, "r");
	if (f) {
		fseek(f, offset*sizeof(struct __fdb_entry), SEEK_SET);
		n = fread(fe, sizeof(struct __fdb_entry), num, f);
		fclose(f);
	} else {
		/* old kernel, use ioctl */
		unsigned long args[4] = { BRCTL_GET_FDB_ENTRIES,
					  (unsigned long) fe,
					  num, offset };
		struct ifreq ifr;
		int retries = 0;

		strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
		ifr.ifr_data = (char *) args;

	retry:
		n = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);

		/* table can change during ioctl processing */
		if (n < 0 && errno == EAGAIN && ++retries < 10) {
			sleep(0);
			goto retry;
		}
	}

	for (i = 0; i < n; i++) 
		__copy_fdb(fdbs+i, fe+i);

	return n;
}

#ifdef TCSUPPORT_IGMP_SNOOPING
int br_set_igmpsnoop_state(const char *br, int igmpsnoop_state)
{
	return br_set(br, "igmpsnoop_state", igmpsnoop_state, BRCTL_SET_IGMPSNOOPING_STATE);
}

int br_set_igmpsnoop_ageing_time(const char *br, struct timeval *tv)
{
	return br_set(br, "igmpsnoop_ageing_time", __tv_to_jiffies(tv),
		      BRCTL_SET_IGMPSNOOPING_AGEING_TIME);
}

int br_set_igmpsnoop_quickleave(const char *br, int igmpsnoop_quickleave)
{
	return br_set(br, "igmpsnoop_quickleave", igmpsnoop_quickleave, BRCTL_SET_IGMPSNOOPING_QUICKLEAVE);
}

#ifndef TCSUPPORT_IGMPSNOOPING_ENHANCE
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,18,21)
int br_set_igmpsnoop_routeportflag(const char *br, int igmpsnoop_routeportflag)
{
	return br_set(br, "igmpsnoop_routeportflag", igmpsnoop_routeportflag, BRCTL_SET_IGMPSNOOPING_ROUTEPORTFLAG);
}
#endif
#endif
int br_set_igmpsnoop_dbg(const char *br, int igmpsnoop_dbg)
{
	return br_set(br, "igmpsnoop_dbg", igmpsnoop_dbg, BRCTL_SET_IGMPSNOOPING_DBG);
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_IGMP_SET_GROUP)
int br_set_igmpsnoop_group_count(const char *br, int igmpsnoop_maxgroup)
{
	return br_set(br, "igmpsnoop_maxgroup", igmpsnoop_maxgroup, BRCTL_SET_IGMPSNOOPING_MAX_GROUP);
}
#endif/*TCSUPPORT_COMPILE*/

static inline void __copy_mc_fdb(struct mc_fdb_entry *ent, 
			      const struct __mc_fdb_entry *f)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	memcpy(ent->group_addr, f->group_addr, 40);
#else
	memcpy(ent->group_addr, f->group_addr, 16);
#endif
	memcpy(ent->host_addr, f->host_addr, 6);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)	
	memcpy(ent->group_mac, f->group_mac, 6);
	ent->version = f->version;
#endif	
	#ifdef TCSUPPORT_IGMP_SNOOPING_V3
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)	
	memcpy(ent->src_addr,f->src_addr,40);
#else
	memcpy(ent->src_addr,f->src_addr,16);
#endif
	ent->filter_mode = f->filter_mode;
	#endif
	ent->port_no = f->port_no;
	__jiffies_to_tv(&ent->ageing_timer_value, f->ageing_timer_value);
}

int br_read_mc_fdb(const char *bridge, struct mc_fdb_entry *fdbs, 
		unsigned long offset, int num)
{
	int i, fd = -1, n;
	struct __mc_fdb_entry fe[num];

	{
		/* old kernel, use ioctl */
		unsigned long args[4] = { BRCTL_GET_MC_FDB_ENTRIES,
					  (unsigned long) fe,
					  num, offset };
		struct ifreq ifr;
		int retries = 0;

		strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
		ifr.ifr_data = (char *) args;

	retry:
		n = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);

		/* table can change during ioctl processing */
		if (n < 0 && errno == EAGAIN && ++retries < 10) {
			sleep(0);
			goto retry;
		}
	}

	for (i = 0; i < n; i++) 
		__copy_mc_fdb(fdbs+i, fe+i);

	if (fd > 0)
		close(fd);
	
	return n;
}
#endif

