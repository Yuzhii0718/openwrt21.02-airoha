/*********************************************************************************
 * LED Kernel xDSL/VOIP dev Trigger
 *
 * Toggles the LED to reflect the link state of a xDSL device
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *********************************************************************************/

#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/leds.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include "tcledctrl.h"
#include <linux/version.h>

/*********************************************************************************
 * micro definition setction
 *
 *********************************************************************************/
#define	DSL_TIMER_INTERVAL	500


/*********************************************************************************
 * data type definition setction
 *
 *********************************************************************************/
struct dev_data {
	rwlock_t lock;
	unsigned interval;
	unsigned link_state;
	struct timer_list timer;
	struct notifier_block notify;
	struct led_classdev *led_cdev;
};

static unsigned char dsl_phy_state = DSL_DOWN;

/*********************************************************************************
 * function prototype setction
 *
 *********************************************************************************/

static void set_dev_state(struct dev_data *trigger_data)
{
	if (DSL_SYNCING == trigger_data->link_state)
	{
		mod_timer(&trigger_data->timer, jiffies + trigger_data->interval);
	}
	else
	{
		led_set_brightness(trigger_data->led_cdev,(trigger_data->link_state) ? LED_FULL : LED_OFF);
		del_timer(&trigger_data->timer);
	}
}

static ssize_t led_interval_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = NULL;
	struct dev_data *trigger_data = NULL;

	led_cdev = dev_get_drvdata(dev);
	trigger_data = led_cdev->trigger_data;
	if (NULL == trigger_data)
	{
		return -ENODEV;
	}

	read_lock(&trigger_data->lock);
	sprintf(buf, "%u\n", jiffies_to_msecs(trigger_data->interval));
	read_unlock(&trigger_data->lock);

	return strlen(buf) + 1;
}

static ssize_t led_interval_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	int ret = -EINVAL;
	size_t count;
	unsigned long value;
	char *after = NULL;
	struct led_classdev *led_cdev = NULL;
	struct dev_data *trigger_data = NULL;

	led_cdev = dev_get_drvdata(dev);
	trigger_data = led_cdev->trigger_data;
	if (NULL == trigger_data)
	{
		return -ENODEV;
	}

	value = simple_strtoul(buf, &after, 10);
	count = after - buf;

	if (isspace(*after))
	{
		count++;
	}

	/* impose some basic bounds on the timer interval */
	if (count == size && value >= 5 && value <= 10000)
	{
		write_lock(&trigger_data->lock);
		trigger_data->interval = msecs_to_jiffies(value);
		/* resets timer */
		set_dev_state(trigger_data);
		write_unlock(&trigger_data->lock);
		ret = count;
	}

	return ret;
}

static DEVICE_ATTR(interval, 0644, led_interval_show, led_interval_store);


static int dev_trig_notify(struct notifier_block *nb,unsigned long evt,void *dv)
{
	struct dev_data *trigger_data = container_of(nb,struct dev_data,notify);

	if ((evt != DSL_UP) && (evt != DSL_DOWN) && (evt != DSL_SYNCING))
	{
		return NOTIFY_DONE;
	}
	dsl_phy_state = evt;
	write_lock(&trigger_data->lock);
	trigger_data->link_state = dsl_phy_state;
	set_dev_state(trigger_data);
	write_unlock(&trigger_data->lock);

	return NOTIFY_DONE;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
static void dev_trig_timer(unsigned long arg)
#else
static void dev_trig_timer(struct timer_list *t)
#endif
{
	unsigned long brightness;
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
	struct dev_data *trigger_data = (struct dev_data *)arg;
    #else
    struct dev_data *trigger_data = from_timer(trigger_data, t, timer);
    #endif

	if (NULL == trigger_data)
	{
		printk("trigger_data is NULL\n");
		return;
	}

	write_lock(&trigger_data->lock);
	if (trigger_data->link_state != DSL_SYNCING)
	{
		/* no need to do timer, just reflect link state. */
		led_set_brightness(trigger_data->led_cdev,(trigger_data->link_state) ? LED_FULL : LED_OFF);
		write_unlock(&trigger_data->lock);

		return;
	}

	brightness = trigger_data->led_cdev->brightness;
	if (!brightness)
	{

		brightness = LED_FULL;
	}
	else
	{
		brightness = LED_OFF;
	}

	led_set_brightness(trigger_data->led_cdev,brightness);

	mod_timer(&trigger_data->timer, jiffies + trigger_data->interval);

	write_unlock(&trigger_data->lock);
}

static void dev_trig_activate(struct led_classdev *led_cdev)
{
	struct dev_data *trigger_data = NULL;
	struct econet_led * ecnt_led = NULL;
	int rc;

	trigger_data = kzalloc(sizeof(struct dev_data), GFP_KERNEL);
	if (NULL == trigger_data)
	{
		return;
	}

	rwlock_init(&trigger_data->lock);

	trigger_data->notify.notifier_call = dev_trig_notify;
	trigger_data->notify.priority = 10;

	/* init the timer */
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
	setup_timer(&trigger_data->timer,dev_trig_timer,(unsigned long)trigger_data);
    #else
    timer_setup(&trigger_data->timer, dev_trig_timer, 0);
    #endif

	trigger_data->led_cdev = led_cdev;
	trigger_data->interval = msecs_to_jiffies(DSL_TIMER_INTERVAL);
	trigger_data->link_state = dsl_phy_state;
	led_cdev->trigger_data = trigger_data;

	rc = device_create_file(led_cdev->dev, &dev_attr_interval);
	if (rc)
	{
		led_cdev->trigger_data = NULL;
		kfree(trigger_data);

		return;
	}


	register_led_notifier(led_cdev->dev->kobj.name,&trigger_data->notify);

	return;
}

static void dev_trig_deactivate(struct led_classdev *led_cdev)
{
	struct dev_data *trigger_data = led_cdev->trigger_data;
	struct econet_led * ecnt_led = NULL;

	if (trigger_data)
	{
		unregister_led_notifier(led_cdev->dev->kobj.name,&trigger_data->notify);

		device_remove_file(led_cdev->dev, &dev_attr_interval);
		del_timer_sync(&trigger_data->timer);

		kfree(trigger_data);
	}
}

static struct led_trigger dev_led_trigger = {
	.name     = "dsldev",
	.activate = dev_trig_activate,
	.deactivate = dev_trig_deactivate,
};

int __init dev_trig_init(void)
{
	return led_trigger_register(&dev_led_trigger);
}

void __exit dev_trig_exit(void)
{
	led_trigger_unregister(&dev_led_trigger);
}

EXPORT_SYMBOL(dev_trig_init);
EXPORT_SYMBOL(dev_trig_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("xDSL PHY LED trigger");