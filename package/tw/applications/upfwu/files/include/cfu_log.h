#ifndef CFU_LOG_H_
#define CFU_LOG_H_
/************
 *
 * Filename:	cfu_log.h
 *
 * Purpose:	Common logging definitions
 *
 * Copyright:	(c) Netgear Inc.
 *		2023 All rights reserved
 *
 ************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * If one wishes to redirect logging, they can by overwriting debug ptr
 * e.g.
 *      debug = myCustomLogFunction;
 */
enum logLev {
    LIB_ERROR, LIB_DEBUG, LIB_INFO
};

/**
 * Handler for a custom debug logging function. The name fw_debug is used to retain
 * compatibility with the original fwcheck code.
 * @param ll logging level
 * @param fmt format string
 * @param ...  format string arguments
 */
void (* fw_debug)(enum logLev ll, const char* fmt, ...);

#ifdef __cplusplus
};
#endif
#endif
