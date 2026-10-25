/*******************************************************************************
 *                      P U B L I C   D A T A
 *******************************************************************************
 */
#include "FreeRTOS.h"


#define ATED_CLI_ENTRY \
	{ "ated_on", "Turn on ated", ated_on, NULL },\
	{ "ated_off", "Turn off ated", ated_off, NULL},\
	{ "ated_exit_wifi_test_mode", "Wifi exit test mode", ated_exit_wifi_test_mode, NULL },\
	{ "ated_set_trans_len", "Set transmit len", ated_set_trans_len, NULL},\
	{ "ated_set_padding", "Set padding", ated_set_padding, NULL},\
	{ "ated_test_write", "Test uart DUT to PC", ated_test_write, NULL},

/*******************************************************************************
 *                      F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */
uint8_t ated_on(uint8_t argc, char *argv[]);
uint8_t ated_off(uint8_t argc, char *argv[]);
uint8_t ated_set_trans_len(uint8_t argc, char *argv[]);
uint8_t ated_set_padding(uint8_t argc, char *argv[]);
uint8_t ated_exit_wifi_test_mode(uint8_t argc, char *argv[]);
uint8_t ated_test_write(uint8_t argc, char *argv[]);

BaseType_t ated_init_task(void);
