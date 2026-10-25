#ifndef _QOSCMD_H_
#define _QOSCMD_H_

//#define 	QOS_DEBUG				1
/* used for speed excute qos switch */
//#define	SPEED_QOS_SWITCH
/* if def this macro, rule is by index, it means rule0 is prio to rul1  */
//#define QOS_RULE_BY_INDEX

#define QOSCMD_LOCK_FILE 			"/tmp/qoslockfd"
#define MAX_RULE_LENGTH				128
#define MAX_APP_RULE_NUM			4
#define	WRR_QUEUE_NUM				4
#define	CAR_QUEUE_NUM				6

#define MAX_QOS_PARAM_LENGTH		(128)


#define	SW_ON		1
#define   SW_OFF		0
#define  TYPE_ADD 	1
#define  TYPE_DEL	0
#define 	MAX_TYPE_PARAM_NUM 	16

enum IPVersion{
	IPNULL_FLAG,
	IPV4_FLAG,
	IPV6_FLAG,
	IPALL_FLAG,
};

typedef struct typeParam_s{
	char  paramName[32];
	char  value[MAX_QOS_PARAM_LENGTH];	
}typeParam_t;

typedef struct appParam_s{
	char appName[32];
	char dstIP[64];
	char dstProto[32];
	int dstPort;
	char queueNum[32];
}appParam_t;


typedef unsigned long  		  		uint32;

typedef struct param_s{
	int 		flag;
	char 		*argname;
	char 		*ipv4Cmd;
	char		*ipv6Cmd;
	char 		value[MAX_QOS_PARAM_LENGTH];	/* 64 is enough for store ipv6 addr */
}param_t;

#endif // _QOSCMD_H_

