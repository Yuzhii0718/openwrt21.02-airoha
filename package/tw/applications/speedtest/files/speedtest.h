#ifndef __SPEEDTEST_H__
#define __SPEEDTEST_H__

#include "fwk/fwk.h"

#define OOKLA_SPEED_TEST_STATUS_FILE        "/tmp/speedtest/ookla_speed_test_status"
#define OOKLA_SPEED_TEST_RESULT_FILE        "/tmp/speedtest/ookla_speed_test_result"
#define OOKLA_SPEED_TEST_LAST_RESULT_FILE   "/tmp/speedtest/ookla_speed_test_last_result"

typedef struct {
    int   nTestTime;
    float fUplinkBandwidth;
    float fDownlinkBandwidth;
    float fAveragePing;         //same as Latency
    int   nResponseCode;
} TSpeedTestResult;

/*
 * Action
 * 0: start
 * 1: cancel
 * 
 * ResponseCode
 * 0 (No error) still running also return 0
 * 401 (Invalid action) 
 * 501 (Action failed)

 * NewErrorCode
 * 1: process already running 
 * 2: result already available
 */
int speedtest_setOOKLASpeedTestStart(int nAction, int *pnResponseCode, int *pnNewErrorCode);

/*
 * NewOOKLAUplinkBandwidth
 * Return uplink bandwidth specified in Mbps
 * 
 * NewOOKLADownlinkBandwidth
 * Return downlink bandwidth specified in Mbps
 * 
 * AveragePing
 * Return ping value in millisecond(For ookla binary support show runtime bandwidth only)
 * 
 * ResponseCode
 * 0 (No error)
 * 1 (processing still running) (If ookla binary support show runtime bandwidth , it need to return the Uplink/Downlink/Averagping.
 *    If binary not support show runtime bandwidth, NewOOKLAUplinkBandwidth and NewOOKLADownlinkBandwidth return 0)
 * 2 (result invalid) result expired or no result, need to re-sent start
 *   (Which mean the test result expired and need return expired NewOOKLAUplinkBandwidth and NewOOKLADownlinkBandwidth)
 * 401 (Invalid action)
 * 501 (Action failed)
 * 
 * [2020-02-27 10:17:00.083] [error] Configuration - Cannot retrieve configuration document (404)
 * [2020-02-27 10:17:00.084] [error] ConfigurationError - Could not retrieve or read configuration (Configuration)
 * [2020-02-27 10:17:00.084] [error] ConfigurationError - Could not retrieve or read configuration (Configuration)
 * [2020-02-27 10:17:00.084] [error] Configuration - Could not retrieve or read configuration (ConfigurationError)
 * 
 */
int speedtest_getOOKLASpeedTestResult(TSpeedTestResult *ptResult);

int speedtest_getOOKLASpeedTestLastResult(TSpeedTestResult *ptResult);

int speedtest_getOOKLASpeedTestStatus(void);

#endif /* __SPEEDTEST_H__ */

