#ifndef _UHTTPD_ENV_

#include <string.h>
struct env_config {
	char *env_name;
	char *conf_name;
	char value[64];
};

void init_product_envs(void);
void set_product_envs(void);

#endif
