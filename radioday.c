#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "sunriset.h"

int main(int argc, char *argv[])
{
	char grid[6];
	char *sYear = (char *)malloc(4);
	char *sMonth = (char *)malloc(2);
	char *sDay = (char *)malloc(2);
	char *sHour = (char *)malloc(2);
	char *sMinute = (char *)malloc(2);
	int i;
	int N, N1, N2, N3;
	int day = 6, month = 6, year = 2019;
	double curT = 18.46;
	double lngHour, longitude, latitude;
	double Lquadrant, RAquadrant;
	double t, Hr, Hs, Ts, Tr, L, M, RA, UTs, UTr;
	double sinDec, cosDec, cosH, zenith;
	double toR = PI / 180;
	double toD = 180 / PI;
	double grayD = 2;
	double riseDist, setDist;
	int rs;

	if (argc < 2 ||
		strlen(argv[1]) != 6 ||
		((argc == 4) && strlen(argv[3]) != 4)) {
		printf("Usage: ./radioday GRID [GRAYDURATION] [HHMM] [YYYYMMDD]\n");
		return 0;
	}

// 	Make grid locator all uppercase
	for  (i = 0; i < 6; i++)
		grid[i] = toupper(argv[1][i]);

//	Fetch grayline duration from command line
	if (argc > 2)
		grayD = atof(argv[2]);

//	Take time from command line if it's there
	if (argc > 3) {
		strncpy(sHour, argv[3], 2);
		strncpy(sMinute, argv[3] + 2, 2);
		curT = atof(sHour) + atof(sMinute) / 60.;
	}

//	Take date from command line if it's there
	if (argc > 4) {
		strncpy(sYear, argv[4], 4);
		year = atoi(sYear);
		strncpy(sMonth, argv[4] + 4, 2);
		month = atoi(sMonth);
		strncpy(sDay, argv[4] + 6, 2);
		day = atoi(sDay);
	}

//	printf("Current time in decimal: %f\n", curT);


	printf("Grayline duration in decimal: %.1fh\n", grayD);
//	printf("Grid locator: %s\n", grid);

	latitude = (grid[1] - 65) * 10 + (grid[3] - 48) + ((double)grid[5] - 65.) / 24 + 1. / 48 - 90;
	longitude = (grid[0] - 65) * 20 + (grid[2] - 48) * 2 + ((double)grid[4] - 65.) / 12 + 1. / 24 - 180;


	printf("Latitude = %f deg. Longitude = %f deg. Date: %04d-%02d-%02d\n",
		latitude, longitude, year, month, day);


	rs = sun_rise_set(year, month, day, longitude, latitude, &UTr, &UTs);

	setDist = fabs(curT - UTs);
	setDist = (setDist > 12.0) ? 24 - setDist : setDist;
	setDist = (setDist <  0.0) ? 24 + setDist : setDist;

	riseDist = fabs(curT - UTr);
	riseDist = (riseDist > 12.0) ? 24 - riseDist : riseDist;
	riseDist = (riseDist <  0.0) ? 24 + riseDist : riseDist;

	printf("Distance to sunrise: %.2f to sunset: %.2f\n", riseDist, setDist);

// 10. convert UT value to local time zone of latitude/longitude

	printf("Sunrise: %02d:%02dZ Sunset: %02d:%02dZ\n", (int)(floor(UTr)), 
		(int)(60 * (UTr - floor(UTr))),	(int)(floor(UTs)), (int)(60 * (UTs - floor(UTs))));

	if (riseDist < grayD / 2.0 || setDist < grayD / 2.0)
		printf("In grayline\n");
	else if (curT > UTr && curT < UTs)
		printf("In daylight\n");
	else
		printf("At night\n");

	return 0;
}

