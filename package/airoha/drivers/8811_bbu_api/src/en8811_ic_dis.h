#ifndef _EN8811_IC_DIS_H_
#define _EN8811_IC_DIS_H_

/*new xfi arch and phy api after 7523*/
#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_XFI_NEW_ARCH 1
#else
#define SUPPORT_XFI_NEW_ARCH 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_SERDES_PON 1
#else
#define SUPPORT_SERDES_PON 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_PCIE0 1
#else
#define SUPPORT_SERDES_PCIE0 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_PCIE1 1
#else
#define SUPPORT_SERDES_PCIE1 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_USB 1
#else
#define SUPPORT_SERDES_USB 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_ETHER 1
#else
#define SUPPORT_SERDES_ETHER 0
#endif

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_BOARD_SELECT 1
#else
#define SUPPORT_BOARD_SELECT 0
#endif

#endif /* _EN8811_IC_DIS_H_ */
