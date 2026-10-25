/*********************************************************************************
 * LEDs notify chain for DSL and VOIP
 *
 * Copyright (C) 2006 Trendchip Technologies, Corp.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *********************************************************************************/
#include <linux/notifier.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/list.h>
#include <linux/slab.h>

extern int led_get_gpio(uint8 *gpio,char led_name[]);

#define MAX_LEDNO_NUM 128
#define GPIONAME_LEN 32
struct LedNotifierList{
	char gpio_name[GPIONAME_LEN];
	uint8 gpio;
	struct raw_notifier_head name;
	struct list_head list;
};

/*struct LedNotifierList ledNotList;*/

struct LedNotifierList *pos_list[MAX_LEDNO_NUM];
/*
int init_led_notifier_list(void)
{
	INIT_LIST_HEAD(&ledNotList.list);
	return 0;
}*/

int register_led_notifier(char *name,struct notifier_block *nb)
{
	struct LedNotifierList *pos = NULL;
	uint8 gpio = -1;
	int i = 0;

	/*list_for_each_entry(pos,&ledNotList.list,list){*/
	for (i = 0; i < MAX_LEDNO_NUM; i++){
		if(pos_list[i] != NULL ) {
			if(0 == strncmp(name, pos_list[i]->gpio_name, GPIONAME_LEN-1))
			{
				printk("name:%s notifier already exist!\n",name);
				return -1;
			}
		}
	}

	if ((!led_get_gpio(&gpio,name)) || (gpio == (uint8)(-1)))
		return -1;

	pos = kmalloc(sizeof(struct LedNotifierList), GFP_ATOMIC);
	if(NULL == pos)
	{
		printk("--> kmalloc failed \n");
		return -1;
	}
	pos_list[gpio] = pos;

	strncpy(pos->gpio_name, name, GPIONAME_LEN-1);
	pos->gpio = gpio;
	pos->name.head = NULL;

	/*list_add_tail(&(pos->list),&ledNotList.list);*/

	return raw_notifier_chain_register(&pos->name, nb);
}

int unregister_led_notifier(char *name,struct notifier_block *nb)
{
	struct LedNotifierList *pos = NULL;
	int ret = -1;
	int i = 0;

	/*list_for_each_entry(pos,&ledNotList.list,list){*/
	for (i = 0; i < MAX_LEDNO_NUM; i++){
		if(pos_list[i] != NULL ) {
			if(0 == strncmp(name, pos_list[i]->gpio_name, GPIONAME_LEN-1) )
			{
				ret = raw_notifier_chain_unregister(&pos_list[i]->name, nb);
				/*list_del(&(pos_list[i]->list));*/
				pos_list[i]=NULL;
				kfree(pos_list[i]);
			return ret;
			}
		}
	}
	return ret;
}

int call_led_notifier(uint8 gpio, unsigned long val)
{
	/*struct LedNotifierList *pos = NULL;*/
	int ret = -1;

	if(pos_list[gpio] != NULL) {
		/*led is exsit*/
		ret= raw_notifier_call_chain(&pos_list[gpio]->name,val,NULL);
	}

	return ret;
}

EXPORT_SYMBOL(register_led_notifier);
EXPORT_SYMBOL(unregister_led_notifier);
EXPORT_SYMBOL(call_led_notifier);

