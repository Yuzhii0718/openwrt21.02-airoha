/**
 * Filename: version.h
 *
 * Purpose: Versioning for the upfwu module
 *
 * Copyright: (c) Netgear Inc.
 *            2017, 2024 All rights reserved
 */

#ifndef VERSION_H_
#define VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_VERSION
#error "MODULE_VERSION is not defined. Please define it during compilation."
#endif

#define UPFWU_VERSION MODULE_VERSION
#define UPFWU_VERSION_SUFFIX ""

#ifdef __cplusplus
}
#endif

#endif /* VERSION_H_ */
