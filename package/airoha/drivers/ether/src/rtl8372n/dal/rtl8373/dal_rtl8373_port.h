#ifndef __DAL_RTL8373_PORT_H__
#define __DAL_RTL8373_PORT_H__


#include <port.h>
#include <dal_rtl8373_switch.h>
#include <linux/string.h>
#include <rtl8221_typedef.h>



typedef enum rtk_iol_mode_e
{
    IOL_MODE_MDI = 0,
    IOL_MODE_MDIX,
    IOL_MODE_END,

} rtk_iol_mode_t;


// typedef struct rtk_rtct_ge_result_s
// {
// 	rtk_uint32  channelALen;
// 	rtk_uint32  channelBLen;
// 	rtk_uint32  channelCLen;
// 	rtk_uint32  channelDLen;
	
// 	rtk_uint32  channelAShort;
// 	rtk_uint32  channelBShort;
// 	rtk_uint32  channelCShort;
// 	rtk_uint32  channelDShort;
	
// 	rtk_uint32  channelAOpen;
// 	rtk_uint32  channelBOpen;
// 	rtk_uint32  channelCOpen;
// 	rtk_uint32  channelDOpen;
	
// 	rtk_uint32  channelAMismatch;
// 	rtk_uint32  channelBMismatch;
// 	rtk_uint32  channelCMismatch;
// 	rtk_uint32  channelDMismatch;
	
// 	rtk_uint32  channelAPairBusy;
// 	rtk_uint32  channelBPairBusy;
// 	rtk_uint32  channelCPairBusy;
// 	rtk_uint32  channelDPairBusy;

// }rtk_rtct_ge_result_t;


// typedef struct rtk_rtct_fe_result_s
// {
// 	rtk_uint32  rxLen;
// 	rtk_uint32  txLen;

// }rtk_rtct_fe_result_t;


// typedef struct rtk_rtct_un_s
// {
// 	rtk_rtct_ge_result_t  ge_result;
// 	rtk_rtct_fe_result_t  fe_result;

// }rtk_rtct_un_t;

// typedef struct rtk_rtctResult_s
// {
// 	rtk_int8  stsResult[40];
// 	rtk_port_speed_t  linkType;
// 	rtk_rtct_un_t  un;

// }rtk_rtctResult_t;


/* Function Name:
 *      dal_rtl8373_portFrcAblitiy_set
 * Description:
 *      Set port force ability
 * Input:
 *      port     - 0 ~ 9
 *      ability    - link, speed, duplex, rxpause, txpause
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portFrcAbility_set(rtk_uint32 port, rtk_port_ability_t* ability);


/* Function Name:
 *      dal_rtl8373_portFrcAblitiy_get
 * Description:
 *      Get port force ability
 * Input:
 *      port     - 0 ~ 9
 *      ability    - link, speed, duplex, rxpause, txpause
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portFrcAbility_get(rtk_uint32 port, rtk_port_ability_t* ability);



/* Function Name:
 *      dal_rtl8373_portStatus_get
 * Description:
 *      Get port status
 * Input:
 *      port     - 0 ~ 9
 *      ability    - link, speed, duplex, rxpause, txpause
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portStatus_get(rtk_uint32 port, rtk_port_status_t* status);

/* Function Name:
 *      dal_rtl8373_portMaxLen_set
 * Description:
 *      Set port rx max length
 * Input:
 *      port     - 0 ~ 9
 *      type    - 0 FE:10M,100M,   1 giga:1G, 2.5G, 5G, 10G
 *      len      - max length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portMaxLen_set(rtk_uint32 port, rtk_uint32 type, rtk_uint32 len);


/* Function Name:
 *      dal_rtl8373_portMaxLen_get
 * Description:
 *      Get port rx max length
 * Input:
 *      port     - 0 ~ 9
 *      type    - 0 FE:10M,100M,   1 giga:1G, 2.5G, 5G, 10G
 *      pLen    - max length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portMaxLen_get(rtk_uint32 port, rtk_uint32 type, rtk_uint32* pLen);




/* Function Name:
 *      dal_rtl8373_portMaxLenIncTag_set
 * Description:
 *      Set port rx max length include tag length: cputag ctag, stag
 * Input:
 *      port     - 0 ~ 9
 *      enaleb  - 1: include tag length,   0: not include tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portMaxLenIncTag_set(rtk_uint32 port, rtk_uint32 enable);


/* Function Name:
 *      dal_rtl8373_portMaxLenIncTag_get
 * Description:
 *      Get port rx max length include tag length: cputag ctag, stag
 * Input:
 *      port     - 0 ~ 9
 *      enaleb  - 1: include tag length,   0: not include tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portMaxLenIncTag_get(rtk_uint32 port, rtk_uint32* pEnable);


/* Function Name:
 *      dal_rtl8373_portLoopbackEn_set
 * Description:
 *      Set port mac tx loopback to rx
 * Input:
 *      port     - 0 ~ 9
 *      enable    - 0 disable loopback,   1 enable loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portLoopbackEn_set(rtk_uint32 port, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl8373_portLoopbackEn_get
 * Description:
 *      Get port mac tx loopback to rx
 * Input:
 *      port     - 0 ~ 9
 *      pEnable    - 0 disable loopback,   1 enable loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portLoopbackEn_get(rtk_uint32 port, rtk_enable_t * pEnable);


/* Function Name:
 *      dal_rtl8373_portBackpressureEn_set
 * Description:
 *      Set port half back pressure enable
 * Input:
 *      port     - 0 ~ 9
 *      enable    - 0 disable backpressure,   1 enable backpressure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portBackpressureEn_set(rtk_uint32 port, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl8373_portBackpressureEn_get
 * Description:
 *      Get port half back pressure enable
 * Input:
 *      port     - 0 ~ 9
 *      pEnable    - 0 disable backpressure,   1 enable backpressure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_portBackpressureEn_get(rtk_uint32 port, rtk_enable_t * pEnable);


/* Function Name:
 *      dal_rtl8373_rtct_init
 * Description:
 *      Init RTCT
 * Input:
 *      port          - port id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid port mask
 * Note:
 *      RTCT test takes 4.8 seconds at most.
 */

extern ret_t dal_rtl8373_rtct_init(rtk_port_t port);



/* Function Name:
 *      dal_rtl8373_rtct_start
 * Description:
 *      Set RTCT start
 * Input:
 *      portmask    - Port mask of RTCT enabled (4 - 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid port mask
 * Note:
 *      RTCT test takes 4.8 seconds at most.
 */
extern ret_t dal_rtl8373_rtct_start(rtk_uint32 phymask);


/* Function Name:
 *      dal_rtl8373_rtct_status_get
 * Description:
 *      Get RTCT result
 * Input:
 *      port    - Port id of RTCT result (4 - 7)
 *      pResult  - result of RTCT
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      RTCT test takes 4.8 seconds at most.
 */
extern ret_t dal_rtl8373_rtct_status_get(rtk_port_t port, rtk_rtct_result_t * pResult);


/* Function Name:
 *      dal_rtl8373_iol_fix_pattern
 * Description:
 *      Set IOL fix pattern.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_fix_pattern(rtk_uint32 port);


/* Function Name:
 *      dal_rtl8373_iol_random_pattern
 * Description:
 *      Set IOL ramdom pattern.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_random_pattern(rtk_uint32 port);



/* Function Name:
 *      dal_rtl8373_iol_10M_mode
 * Description:
 *      Set IOL 10M mode.
 * Input:
 *      port  - port id 0 - 8 
 *      mode - 0:mdi   1:mdix
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_10M_mode(rtk_uint32 port, rtk_uint32 mode);


/* Function Name:
 *      dal_rtl8373_iol_100M_mode
 * Description:
 *      Set IOL 100M mode.
 * Input:
 *      port  - port id 0 - 8 
 *      mode - 0:mdi   1:mdix
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_100M_mode(rtk_uint32 port, rtk_uint32 mode);



/* Function Name:
 *      dal_rtl8373_iol_giga_mode
 * Description:
 *      Set IOL Giga mode.
 * Input:
 *      port  - port id 0 - 8 
 *      mode - 1 - 4
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_giga_mode(rtk_uint32 port, rtk_uint32 mode);

/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode1
 * Description:
 *      Set IOL 2.5G mode1.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode1(rtk_uint32 port);


/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode2
 * Description:
 *      Set IOL 2.5G mode2.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode2(rtk_uint32 port);


/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode3
 * Description:
 *      Set IOL 2.5G mode3.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode3(rtk_uint32 port);


/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode4
 * Description:
 *      Set IOL 2.5G mode4.
 * Input:
 *      port  - port id 0 - 8 
 *      tone - 1-5
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode4(rtk_uint32 port, rtk_uint32 tone);



/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode5
 * Description:
 *      Set IOL 2.5G mode5.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode5(rtk_uint32 port);



/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode6
 * Description:
 *      Set IOL 2.5G mode6.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */

extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode6(rtk_uint32 port);


/* Function Name:
 *      dal_rtl8373_iol_2p5G_mode7
 * Description:
 *      Set IOL 2.5G mode7.
 * Input:
 *      port  - port id 0 - 8 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_2p5G_mode7(rtk_uint32 port);

/* Function Name:
 *      dal_rtl8373_port_extphyid_set
 * Description:
 *      Set   smi address ,SMI Address, default value equals to port number
 * Input:
 *      sdsid     - 0,1
 *      phyid   - get from phy strap pin
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */


extern ret_t dal_rtl8373_port_extphyid_set(rtk_uint32 sdsid,  rtk_uint32 phyid);

/* Function Name:
 *      dal_rtl8373_port_extphyid_get
 * Description:
 *      Get   smi address ,SMI Address, default value equals to port number
 * Input:
 *      sdsid     - 0,1
 *      phyid   - get smi address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */

extern ret_t dal_rtl8373_port_extphyid_get(rtk_uint32 sdsid,  rtk_uint32 *phyid);
/* Function Name:
 *      dal_rtl8373_sdsMode_set
 * Description:
 *      Set sds mode
 * Input:
 *      sdsid     - 0 ~ 1
 *      mode  - 
 *      SERDES_10GQXG,
 *      SERDES_10GUSXG,
 *      SERDES_10GR,
 *      SERDES_HSG,
 *      SERDES_2500BASEX,
 *      SERDES_SG,
 *      SERDES_1000BASEX,
 *      SERDES_100FX,    
 *      SERDES_OFF, 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_NOT_ALLOWED      - mode is not support
 * Note:
 *      None
 */

extern rtk_api_ret_t dal_rtl8373_sdsMode_set(rtk_uint32 sdsid, rtk_sds_mode_t mode);

/* Function Name:
 *      dal_rtl8373_sdsMode_get
 * Description:
 *      Get sds mode
 * Input:
 *      sdsid     - 0 ~ 1
 *      pMode  - 
 *      SERDES_10GQXG,
 *      SERDES_10GUSXG,
 *      SERDES_10GR,
 *      SERDES_HSG,
 *      SERDES_2500BASEX,
 *      SERDES_SG,
 *      SERDES_1000BASEX,
 *      SERDES_100FX,    
 *      SERDES_OFF, 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_NOT_ALLOWED      - mode is not support
 * Note:
 *      None
 */

extern rtk_api_ret_t dal_rtl8373_sdsMode_get(rtk_uint32 sdsid, rtk_sds_mode_t * pMode);
/* Function Name:
 *      dal_rtl8373_fiber_fc_set
 * Description:
 *      Configure serdes port Nway state
 * Input:
 *      sdsId        - serdes ID
 *      sdsMode      - serdes mode
 *      fc_en       - 1:enable fc, 0:disable fc
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SERDES port Nway state
 */
extern rtk_api_ret_t dal_rtl8373_fiber_fc_set(rtk_uint32 sdsId, rtk_sds_mode_t sdsMode, rtk_enable_t fc_en);
/* Function Name:
 *      dal_rtl8373_fiber_fc_get
 * Description:
 *      Configure serdes port Nway state
 * Input:
 *      sdsId        - serdes ID
 *      sdsMode      - serdes mode
 *      fc_en       - 1:enable FC, 0:disable FC
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SERDES port Nway state
 */
extern rtk_api_ret_t dal_rtl8373_fiber_fc_get(rtk_uint32 sdsId, rtk_sds_mode_t sdsMode, rtk_enable_t *fc_en);

/* Function Name:
 *      dal_rtl8373_port_sdsNway_set
 * Description:
 *      Configure serdes port Nway state
 * Input:
 *      sdsId        - serdes ID
 *      sdsMode      - serdes mode
 *      enable       - 1:enable nway, 0:disable nway
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SERDES port Nway state
 */
extern rtk_api_ret_t dal_rtl8373_port_sdsNway_set(rtk_uint32 sdsId, rtk_sds_mode_t sdsMode, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl8373_port_sdsNway_get
 * Description:
 *      Get serdes Nway
 * Input:
 *      sdsid        - serdes ID
 *      pState       - Nway state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure serdes port Nway state
 */
extern rtk_api_ret_t dal_rtl8373_port_sdsNway_get(rtk_uint32 sdsId, rtk_sds_mode_t sdsMode, rtk_enable_t *pState);

/* Function Name:
 *      dal_rtl8373_rtct_init
 * Description:
 *      Init RTCT
 * Input:
 *      port          - port id of RTCT test (4 - 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid port mask
 * Note:
 *      RTCT test takes 4.8 seconds at most.
 */

extern ret_t dal_rtl8373_rtct_init(rtk_port_t port);

/* Function Name:
 *      dal_rtl8373_port_txPktMaxLen_set
 * Description:
 *      Set the max length of packet that tx port can transmit under difference speed
 * Input:
 *      port     - tx port number
 *      type     - 0 FE:10M,100M,   1 giga:1G, 2.5G, 5G, 10G (tx port work in different speed has different ability)
 *      pktLen   - max tx packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_port_txPktMaxLen_set(rtk_port_t port, rtk_uint32 type, rtk_uint32 pktLen);

/* Function Name:
 *      dal_rtl8373_port_txPktMaxLen_get
 * Description:
 *      Get the max length of packet that tx port can transmit under specify speed
 * Input:
 *      port     - tx port number
 *      type     - 0 FE:10M,100M,   1 giga:1G, 2.5G, 5G, 10G (tx port work in different speed has different ability)
 * Output:
 *      pktLen   - max tx packet length
 * Return:
 *      RT_ERR_OK         - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_port_txPktMaxLen_get(rtk_port_t port, rtk_uint32 type, rtk_uint32 *pPktLen);

/* Function Name:
 *      dal_rtl8373_iol_sd
 * Description:
 *      Set sds test mode.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      prbs_tx -prbs31/prbs9/tx_8081
 *      sdsid -0/1
 *      on_off -0/1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_sd(rtk_uint32 chip, rtk_uint32 prbs_tx, rtk_uint32 sdsid, rtk_uint32 on_off);

/* Function Name:
 *      dal_rtl8373_iol_pre_amp
 * Description:
 *      Set sds test mode.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      pre   -0~63
 *      sdsid -0/1
 *      endis -0/1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_pre_amp(rtk_uint32 chip, rtk_uint32 pre, rtk_uint32 sdsid, rtk_uint32 endis);

/* Function Name:
 *      dal_rtl8373_iol_main_amp
 * Description:
 *      Set sds test mode.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      main   -0~63
 *      sdsid -0/1
 *      boost -0/1
 *      endis -0/1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_main_amp(rtk_uint32 chip, rtk_uint32 main, rtk_uint32 sdsid,rtk_uint32 boost, rtk_uint32 endis);

/* Function Name:
 *      dal_rtl8373_iol_post_amp
 * Description:
 *      Set sds test mode.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      post   -0~63
 *      sdsid -0/1
 *      endis -0/1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_post_amp(rtk_uint32 chip, rtk_uint32 post, rtk_uint32 sdsid, rtk_uint32 endis);

/* Function Name:
 *      dal_rtl8373_iol_txz0
 * Description:
 *      Set sds test mode.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      sdsid -0/1
 *      txz0 -0~15
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_txz0(rtk_uint32 chip,  rtk_uint32 sdsid, rtk_uint32 txz0);

extern void SDS_DEBUG_PAD_OUT(rtk_uint32 sdsid, rtk_uint32 chip);
extern void WTGSDS_DEBUG_SEL(rtk_uint32 sdsid, rtk_uint32 sel, rtk_uint32 chip);
extern rtk_uint32 SDS_BUDEG_OUT_DATA(rtk_uint32 chip);
extern void EyeMonitorMDIOCLK(rtk_uint32 sdsid, rtk_uint32 chip);
extern rtk_uint32 dump_EYE_DATA(rtk_uint32 sdsid, rtk_uint32 dbg_out, rtk_uint32 chip);
/* Function Name:
 *      dal_rtl8373_iol_eye
 * Description:
 *      eye monitor , ��� RTL8373/2 s0/1  �� RTL8373_ind_ac_RTL8224 s0.
 * Input:
 *      chip  - 8373/8373N/8372/8372N/8366U/8224 
 *      sdsid -0/1
 *      frame -0~x
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will enbale MDC function.
 */
extern rtk_api_ret_t dal_rtl8373_iol_eye(rtk_uint32 chip,  rtk_uint32 sdsid, rtk_uint32 frame);

extern rtk_api_ret_t phyrtct_start1(rtk_port_t port);

extern rtk_api_ret_t phyrtct_sts_get(rtk_port_t port, rtk_rtct_result_t* pRtctResult);

extern rtk_api_ret_t phyrtct_length_get(rtk_port_t port, rtk_uint32 channel,rtk_uint32 *t_length);

rtk_uint32 phy_sram_read(rtk_uint32 port,rtk_uint32 addr);

rtk_api_ret_t phyrtct_status_convert(rtk_uint32 phyData, rtk_uint32 *pShort, rtk_uint32 *pOpen, rtk_uint32 *pMismatch, rtk_uint32 *pPairBusy);
rtk_api_ret_t Rtl8221_rtct_start(rtk_port_t port);
rtk_api_ret_t Rtl8221_rtctResult_convert(rtk_uint32 phydata, PHY_RTCT_STATUS *pRtctStatus);
rtk_api_ret_t Rtl8221_rtctResult_get(rtk_port_t port,PHY_RTCT_RESULT *pRtctResult);




#endif

