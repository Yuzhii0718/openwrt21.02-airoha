/*
 *  i2s-evb_register.c
 *
 *  Copyright (c) 2020 Econet Inc
 *
 */

#include <linux/module.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <asm/mach-types.h>


static struct platform_device *econet_i2s_machine;

static int __init econet_i2s_init(void)
{
	int ret;
	char *str;

	econet_i2s_machine = platform_device_alloc("i2s-evb", -1);
	if (!econet_i2s_machine)
		return -ENOMEM;

	ret = platform_device_add(econet_i2s_machine);

	if (ret)
		platform_device_put(econet_i2s_machine);

	return ret;
}
module_init(econet_i2s_init);

static void __exit econet_i2s_exit(void)
{
	platform_device_unregister(econet_i2s_machine);
}
module_exit(econet_i2s_exit);




static struct platform_device *econet_i2s_pcm;

static int __init econet_i2s_pcm_init(void)
{
	int ret;
	char *str;

	econet_i2s_pcm = platform_device_alloc("i2s-afe-pcm", -1);
	if (!econet_i2s_pcm)
		return -ENOMEM;

	ret = platform_device_add(econet_i2s_pcm);

	if (ret)
		platform_device_put(econet_i2s_pcm);

	return ret;
}
module_init(econet_i2s_pcm_init);

static void __exit econet_i2s_pcm_exit(void)
{
	platform_device_unregister(econet_i2s_pcm);
}
module_exit(econet_i2s_pcm_exit);


MODULE_AUTHOR("Yafei.Ren, Yafei.Ren@econet-inc.com");
MODULE_DESCRIPTION("ALSA SoC Econet I2S");
MODULE_LICENSE("GPL");

