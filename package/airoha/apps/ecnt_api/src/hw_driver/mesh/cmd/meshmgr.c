#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libapi_lib_meshmgr.h>
#include <libapi_lib_utility.h>


int main(int argc, char* argv[])
{
	int type = 0;
	if(argc < 2)
	{
		return -1;
	}
	type = atoi(argv[1]);
	
	printf("type = %d\n", type);
	
	meshmgr_lib_notify_cfg(type);
	
	return 0;
}


