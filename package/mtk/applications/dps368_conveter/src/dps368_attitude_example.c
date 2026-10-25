// SPDX-License-Identifier: BSD-3-Clause
/*
 * Caculate delta attitude from pressure and temperature values
 *
 * Copyright (C) 2023 MediaTek Inc.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 *
 * Reference: https://keisan.casio.com/exec/system/1224585971
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NAME "dps368_attitude_example"
#define DESC "caculate delta attitude from pressure and temperature values"

double h(double p0, double p, double t)
{
	double exp = 1.0/5.257;

	return ((pow((p0/p),exp)-1.0)*(t+273.15))/0.0065;
}

int main(int argc, char *argv[])
{
	double P0, P1, P2, T1, T2;
	double H1, H2, dH;
	double input[6];
	int i;

	if (argc < 5) {
		printf("\n%s: %s\n", NAME, DESC);
		printf("\nReference: https://keisan.casio.com/exec/system/1224585971\n");
		printf("\nUsage: %s P1 T1 P2 T2 [P0=101.325]\n", NAME);
		printf("\tP1     : Atmospheric Pressure (kilopascal) of reference area\n");
		printf("\tT1     : Temperature (milli degrees Celsius) of reference area\n");
		printf("\tP2     : Atmospheric Pressure (kilopascal) of target area\n");
		printf("\tT2     : Temperature (milli degrees Celsius) of target area\n");
		printf("\tP0     : Atmospheric Pressure (kilopascal) of sea level area\n");
		printf("\t         If this parameter not set, use average air pressure\n");
		printf("\t         at sea level 101.325 (kilopascal) as default value\n");
		printf("\nExample:\n");
		printf("\tCase1  : %s 100.55 20000.0 100.23 10000.0\n", NAME);
		printf("\tCase2  : %s 100.55 20000.0 100.23 10000.0 101.125\n", NAME);
		printf("\nComment:\n");
		printf("\tRead temperature : cat /sys/bus/iio/devices/*/in_temp_input\n");
		printf("\tRead pressure    : cat /sys/bus/iio/devices/*/in_pressure_input\n");
		exit(-22);
	}

	if (argc > 6)
		argc = 6;

	for (i=1 ; i<argc ; i++)
		input[i] = atof(argv[i]);

	P1 = input[1] * 10.0;
	T1 = input[2] / 1000.0;
	P2 = input[3] * 10.0;
	T2 = input[4] / 1000.0;

	if (argc == 6)
		P0 = input[5] * 10.0;
	else
		P0 = 1013.25;

	H1 = h(P0, P1, T1);
	H2 = h(P0, P2, T2);
	dH = H2 - H1;

	printf("P0=%lf P1=%lf T1=%lf P2=%lf T2=%lf --> H1=%lf H2=%lf dH=%lf\n",
		P0, P1, T1, P2, T2, H1, H2, dH);

	if (dH >= 0)
		if (dH > 1.0)
			printf("+%lfm\n", dH);
		else
			printf("+%lfcm\n", dH*100);
	else
		if (dH < -1.0)
			printf("%lfm\n", dH);
		else
			printf("%lfcm\n", dH*100);
}
