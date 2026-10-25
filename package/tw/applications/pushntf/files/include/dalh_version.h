/************
 *
 *  Filename:	dalh_version.h
 *  Purpose:	versioning of the dalh process
 *  Copyright:  (c) Netgear Inc.
 *  			2019 All rights reserved
 *  Author:     @ VVDN TECHNOLOGIES
 *
 *************/

/* Define to prevent recursive inclusion---------------------*/

#ifndef _DALH_VERSION_H_
#define _DALH_VERSION_H_

#ifndef MODULE_VERSION
#error "MODULE_VERSION is not defined. Please define it during compilation."
#endif

#define DALH_VERSION MODULE_VERSION
#define DALH_VERSION_SUFFIX ""

#endif //_DALH_VERSION_H_
