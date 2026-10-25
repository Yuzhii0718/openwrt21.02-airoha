#ifndef _DBG_CMD_QDMA_H_
#define _DBG_CMD_QDMA_H_

#define QDMA_TYPE_LAN "qdma_lan"
#define QDMA_TYPE_WAN "qdma_wan"

#define QDMAMGR_SUCCESS	(0)
#define QDMAMGR_FAIL	(-1)

enum
{
    RET_SUCCESS = 0,  
    RET_INVALID_PARAM,  
    RET_NOT_SUPPORT,    
    RET_FAIL,     
};

static inline int check_result(int ret)
{
	if(ret == RET_NOT_SUPPORT) 
	{
		printf("NOT SUPPORT\n");
		return QDMAMGR_FAIL;
	}
	else if(ret == RET_FAIL)
	{
		printf("Exec.Failed\n");
		return QDMAMGR_FAIL;
	}
	else if(ret == RET_INVALID_PARAM)
	{
		printf("INVALID PARAM\n");
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;
}

static inline int check_fail(int ret)
{
	if(ret == RET_NOT_SUPPORT) 
	{
		printf("NOT SUPPORT\n");
		return 1;
	}
	else if(ret == RET_FAIL)
	{
		printf("Exec.Failed\n");
		return 1;
	}
	else if(ret == RET_INVALID_PARAM)
	{
		printf("INVALID PARAM\n");
		return 1;
	}

	return 0;
}

typedef enum {
	QDMA_DISABLE = 0 ,
	QDMA_ENABLE
} QDMA_Mode_t ;

#define CONFIG_QDMA_LAN_CHANNEL 8
#define CONFIG_QDMA_WAN_CHANNEL 32
#define CONFIG_QDMA_QUEUE 8

#endif
