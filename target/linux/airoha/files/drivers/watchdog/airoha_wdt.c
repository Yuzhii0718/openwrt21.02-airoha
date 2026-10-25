// SPDX-License-Identifier: GPL-2.0+
/*
 * Airoha Watchdog Driver
 *
 * Copyright (C) 2024 Maso Huang
 *
 * Maso Huang <maso.huang@mediatek.com>
 *
 * Based on mtk_wdt.c
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>

#define CR_TIMER_CTL		0x000
#define CR_TIMER3_LDV		0x2C
#define CR_TIMER3_VLR		0x30
#define CR_WDT_THSLD		0x34
#define CR_WDT_RLD		0x38
#define TIMER3_EN		(1 << 5)
#define WDT_IRQ			(1 << 21)
#define WDT_EN			(1 << 25)
#define WDT_RLD			0x1
/*
 * timercnt = WDT_TIMEOUT *  WDT_TIMETICK_1S * SYS_HCLK * 500
 * 7581/7552's SYS_HCLK(300) is too high that would cause timer cnt overflow
 * thus reduce the WDT_TIMEOUT to aoivd overflow issue. 
 */
#define WDT_MAX_TIMEOUT		28	//s
#define WDT_MIN_TIMEOUT		1	//s
#define WDT_RST_VAL		500	//ms
#define WDT_TIMERTICK_1MS	1
#define WDT_TIMERTICK_1S	1000
#define SYS_HCLK		300

#define DRV_NAME		"airoha-wdt"
#define DRV_VERSION		"1.0"

static bool nowayout = WATCHDOG_NOWAYOUT;
static unsigned int timeout;

struct airoha_wdt_dev {
	struct watchdog_device wdt_dev;
	void __iomem *wdt_base;
};

static int airoha_wdt_ping(struct watchdog_device *wdt_dev)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;

	regmap_write(wdt_base, CR_WDT_RLD, WDT_RLD);

	return 0;
}

static int airoha_wdt_restart(struct watchdog_device *wdt_dev,
			   unsigned long action, void *data)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	u32 reg;

	reg = (WDT_RST_VAL * WDT_TIMERTICK_1MS * SYS_HCLK) * 500;
	regmap_write(wdt_base, CR_TIMER3_LDV, reg);
	airoha_wdt_ping(wdt_dev);

	mdelay(500);

	return 0;
}

static int airoha_wdt_set_timeout(struct watchdog_device *wdt_dev,
				unsigned int timeout)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	u32 reg;

	if (timeout > WDT_MAX_TIMEOUT || timeout < WDT_MIN_TIMEOUT 
			|| timeout <= wdt_dev->pretimeout)
		return -EINVAL;

	wdt_dev->timeout = timeout;

	reg = (timeout * WDT_TIMERTICK_1S * SYS_HCLK) * 500; 
	regmap_write(wdt_base, CR_TIMER3_LDV, reg);

	airoha_wdt_ping(wdt_dev);

	return 0;
}

static int airoha_wdt_set_pretimeout(struct watchdog_device *wdt_dev,
				  unsigned int pretimeout)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	u32 reg;

	if (!pretimeout) {
		wdt_dev->pretimeout = 0;
	} else if (pretimeout < wdt_dev->timeout) {
		wdt_dev->pretimeout = pretimeout;
	} else {
		return -EINVAL;
	}

	reg = (pretimeout * WDT_TIMERTICK_1S * SYS_HCLK) * 500;
	regmap_write(wdt_base, CR_WDT_THSLD, reg);

	return 0;
}

static int airoha_wdt_stop(struct watchdog_device *wdt_dev)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	int ret;
	u32 reg;

	ret = regmap_read(wdt_base, CR_TIMER_CTL, &reg);

	if (ret)
		return ret;

	reg &= ~(TIMER3_EN | WDT_EN);
	regmap_write(wdt_base, CR_TIMER_CTL, reg);

	return 0;
}

static int airoha_wdt_start(struct watchdog_device *wdt_dev)
{
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	int ret;
	u32 reg;

	airoha_wdt_set_timeout(wdt_dev, wdt_dev->timeout);
	if (wdt_dev->pretimeout)
		airoha_wdt_set_pretimeout(wdt_dev, wdt_dev->pretimeout);

	ret = regmap_read(wdt_base, CR_TIMER_CTL, &reg);

	if (ret)
		return ret;

	reg |= (TIMER3_EN | WDT_EN);
	regmap_write(wdt_base, CR_TIMER_CTL, reg);

	return 0;
}

static irqreturn_t airoha_wdt_isr(int irq, void *arg)
{
	struct watchdog_device *wdt_dev = arg;
	struct airoha_wdt_dev *airoha_wdt = watchdog_get_drvdata(wdt_dev);
	void __iomem *wdt_base = airoha_wdt->wdt_base;
	u32 reg;

	regmap_read(wdt_base, CR_TIMER_CTL, &reg);

	reg |= WDT_IRQ;
	regmap_write(wdt_base, CR_TIMER_CTL, reg);

	watchdog_notify_pretimeout(wdt_dev);

	return IRQ_HANDLED;
}

static const struct watchdog_info airoha_wdt_info = {
	.identity	= DRV_NAME,
	.options	= WDIOF_SETTIMEOUT |
			  WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static const struct watchdog_info airoha_wdt_pt_info = {
	.identity	= DRV_NAME,
	.options	= WDIOF_SETTIMEOUT |
			  WDIOF_PRETIMEOUT |
			  WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops airoha_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= airoha_wdt_start,
	.stop		= airoha_wdt_stop,
	.ping		= airoha_wdt_ping,
	.set_timeout	= airoha_wdt_set_timeout,
	.set_pretimeout	= airoha_wdt_set_pretimeout,
	.restart	= airoha_wdt_restart,
};

static int airoha_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct airoha_wdt_dev *airoha_wdt;
	int err, irq;

	airoha_wdt = devm_kzalloc(dev, sizeof(*airoha_wdt), GFP_KERNEL);
	if (!airoha_wdt)
		return -ENOMEM;

	platform_set_drvdata(pdev, airoha_wdt);

	airoha_wdt->wdt_base = syscon_regmap_lookup_by_compatible("econet,ecnt-timer");
	if (IS_ERR(airoha_wdt->wdt_base))
		return PTR_ERR(airoha_wdt->wdt_base);

	irq = platform_get_irq(pdev, 0);
	if (irq > 0) {
		err = devm_request_irq(&pdev->dev, irq, airoha_wdt_isr, 0,
				dev_name(&pdev->dev), &airoha_wdt->wdt_dev);
		if (err)
			return err;

		airoha_wdt->wdt_dev.info = &airoha_wdt_pt_info;
		airoha_wdt->wdt_dev.pretimeout = WDT_MAX_TIMEOUT / 2;
	} else {
		if (irq == -EPROBE_DEFER)
			return -EPROBE_DEFER;

		airoha_wdt->wdt_dev.info = &airoha_wdt_info;
	}

	airoha_wdt->wdt_dev.ops = &airoha_wdt_ops;
	airoha_wdt->wdt_dev.timeout = WDT_MAX_TIMEOUT;
	airoha_wdt->wdt_dev.max_timeout = WDT_MAX_TIMEOUT;
	airoha_wdt->wdt_dev.min_timeout = WDT_MIN_TIMEOUT;
	airoha_wdt->wdt_dev.parent = dev;

	watchdog_init_timeout(&airoha_wdt->wdt_dev, timeout, dev);
	watchdog_set_nowayout(&airoha_wdt->wdt_dev, nowayout);
	watchdog_set_restart_priority(&airoha_wdt->wdt_dev, 128);

	watchdog_set_drvdata(&airoha_wdt->wdt_dev, airoha_wdt);

	airoha_wdt_stop(&airoha_wdt->wdt_dev);

	watchdog_stop_on_reboot(&airoha_wdt->wdt_dev);
	err = devm_watchdog_register_device(dev, &airoha_wdt->wdt_dev);
	if (unlikely(err))
		return err;

	dev_info(dev, "Watchdog enabled (timeout=%d sec, nowayout=%d)\n",
			airoha_wdt->wdt_dev.timeout, nowayout);

	return 0;
};

static const struct of_device_id airoha_wdt_dt_ids[] = {
	{ .compatible = "airoha,an7563-wdt" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, airoha_wdt_dt_ids);

static struct platform_driver airoha_wdt_driver = {
	.probe		= airoha_wdt_probe,
	.driver		= {
		.name		= DRV_NAME,
		.of_match_table	= airoha_wdt_dt_ids,
	},
};

module_platform_driver(airoha_wdt_driver);

module_param(timeout, uint, 0);
MODULE_PARM_DESC(timeout, "Watchdog heartbeat in seconds");

module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
			__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maso Huang <maso.huang@mediatek.com>");
MODULE_DESCRIPTION("Airoha WatchDog Timer Driver");
MODULE_VERSION(DRV_VERSION);
