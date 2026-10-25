#include <linux/types.h>
#include <linux/fs.h>
#include "do_mounts.h"
#include "ecnt_init_ext.h"

/**************************************************************************************************/
/**************************************************************************************************/
int __init ecnt_do_mount_root_hook(char *name, char *fs, int flags, void *data, int*err)
{
	if(*err)
	{
	}
	
	return *err;
}


