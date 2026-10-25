#ifndef __ADVANCED_QOS_H_
#define __ADVANCED_QOS_H_

#define M_ADV_QOS_SETSPEEDTESTSTART         "SetOOKLASpeedTestStart"
#define M_ADV_QOS_GETSPEEDTESTRESULT        "GetOOKLASpeedTestResult"

#define ADV_QOS_GETSPEEDTESTRESULT_RESPFMT "<NewOOKLAUplinkBandwidth>%.2f</NewOOKLAUplinkBandwidth>\r\n\
<NewOOKLADownlinkBandwidth>%.2f</NewOOKLADownlinkBandwidth>\r\n\
<AveragePing>%.2f</AveragePing>\r\n"

int soapPlt_getOOKLASpeedTestResult(char *bodyResp, int bufferLen);
int soapPlt_setOOKLASpeedTestStart(int iAction, int *pRespCode, int *pNewErrorCode);

#endif /* __ADVANCED_QOS_H_ */

