// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2021-2023 Airoha Inc.

#include <linux/kernel.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <asm/suspend.h>
#include <asm/cpuidle.h>
#include <linux/module.h>
#include <asm/proc-fns.h>
#include <linux/time.h>

static int arm64_wfi_enter(struct cpuidle_device *dev, struct cpuidle_driver *drv, int index)
{
	asm volatile("wfi");
	return index;
}

static struct cpuidle_driver airoha_idle_driver = {
	.name			= "airoha_idle",
	.owner			= THIS_MODULE,
	.states = {
#ifndef __aarch64__
		[0] = ARM_CPUIDLE_WFI_STATE,
#else
		[0] = {
			.name = "wfi",
			.desc = "ARM WFI",
			.power_usage = UINT_MAX,
			.enter = arm64_wfi_enter,
			.exit_latency = 1,
			.target_residency = 1,
		},
#endif
	},
	.state_count = 1,
	.safe_state_index = 0,
};



static int __init airoha_cpuidle_probe(void)
{
	int ret;
	
	ret = cpuidle_register(&airoha_idle_driver, NULL);

	if (ret) {
		pr_err("[CPUIDLE] error while loading\n");
		return ret;
	}

	return 0;
}

static void __exit airoha_cpuidle_exit(void) {
	return;
}

module_init(airoha_cpuidle_probe);
module_exit(airoha_cpuidle_exit);

MODULE_AUTHOR("shivang upadhyay");
MODULE_LICENSE("GPL v2");
