#ifndef _LEDCTRL_KERNEL_API_H
#define _LEDCTRL_KERNEL_API_H

/* Function Declarations */
int led_init(void);
void led_exit(void);
int led_init_others(void);
void  led_exit_others(void);

#endif