#ifndef AVS_H
#define AVS_H

typedef unsigned int        u32;

typedef enum
{
	AVS_OK = 0, // OK
	OUT_RANGE_FAIL, 	 // out of range FAIL
	TEMP_HIGH_FAIL,		//temperature too high
	AVS_FAIL
} AVS_STATUS_T; // AVS status type

typedef enum
{
	NV = 0, // disable AVS. i.e. back to normal Vcore of the Buck
	LV, 	 // enable AVS & decrease 50mV(if iddq<=90mA)/100mV(if iddq>90mA)
	HV,		//enable AVS & increase 50 mV(if iddq<30mA)
} V_SEL_T; // AVS status type

extern u32 AVS_Get_Vcore(void);
extern AVS_STATUS_T AVS_Set(u32 V_SEL, u32 iddq);
extern u32 AVS_Get_Affinity(u32 pid, u32 *mask);
extern u32 AVS_Set_Affinity(u32 pid, u32 *mask);
#endif /* AVS_H */
