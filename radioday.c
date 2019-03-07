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
	char *state;
	int i;
	int day = 6, month = 6, year = 2019;
	int curT = 18 * 60; // In minutes
	double longitude, latitude;
	double UTsH, UTrH; // In hours
	int normS, normT, UTs, UTr; // In minutes
	int grayD = 180; // In minutes
	int dayLen, riseDist, setDist; // In minutes
	int rs;
	int verbose = 0;

	if (argc < 2 ||
		strlen(argv[1]) != 6 ||
		((argc == 4) && strlen(argv[3]) != 4)) {
		printf("Usage: ./radioday GRID [GRAYDURATION] [HHMM] [YYYYMMDD]\n");

		free(sYear);
		free(sMonth);
		free(sDay);
		free(sHour);
		free(sMinute);

		return 0;
	}

// 	Make grid locator all uppercase
	for  (i = 0; i < 6; i++)
		grid[i] = toupper(argv[1][i]);

//	Fetch grayline duration from command line
	if (argc > 2) {
		grayD = (int)(atof(argv[2]) * 60.0);
		verbose = (grayD < 0);
		grayD = abs(grayD);
	}


//	Take time from command line if it's there
	if (argc > 3) {
		strncpy(sHour, argv[3], 2);
		strncpy(sMinute, argv[3] + 2, 2);
		curT = (int)(atof(sHour) * 60 + atof(sMinute));
	}

//	Take date from command line if it's there
	if (argc > 4) {
		strncpy(sYear, argv[4], 4);
		year = atoi(sYear);
		strncpy(sMonth, argv[4] + 4, 2);
		month = atoi(sMonth);
		strncpy(sDay, argv[4] + 6, 2);
		day = atoi(sDay);
		if (verbose)
			printf("sYear=\"%s\", sMonth=\"%s\", sDay=\"%s\"\n", sYear, sMonth, sDay);
	}

	rs = sun_rise_set(year, month, day, longitude, latitude, &UTrH, &UTsH);
	latitude = (grid[1] - 65) * 10 + (grid[3] - 48) + ((double)grid[5] - 65.) / 24 + 1. / 48 - 90;
	longitude = (grid[0] - 65) * 20 + (grid[2] - 48) * 2 + ((double)grid[4] - 65.) / 12 + 1. / 24 - 180;
	UTs = round(60.0 * UTsH);
	UTr = round(60.0 * UTrH);

	if (verbose) {
		printf("Current time in decimal: %f\n", curT / 60.0);
		printf("Grayline duration in decimal: %.1fh\n", grayD / 60.0);
		printf("Grid locator: %s\n", grid);
		printf("Latitude = %f deg. Longitude = %f deg. Date: %04d-%02d-%02d\n",
			latitude, longitude, year, month, day);
		printf("rs = %d\n", rs);
	}


	if (rs == 1)
		if (verbose)
			printf("In constant daylight.\n");
		else
			printf("day\n");
	else if (rs == -1)
		if (verbose)
			printf("In constant darkness.\n");
		else
			printf("night\n");
	else {
		dayLen = (UTs - UTr + 1440) % 1440;

		riseDist = (int)(curT - UTr + 2880) % 1440;
		riseDist = (riseDist > 720) ? 1440 - riseDist : riseDist;

		setDist =  (int)(curT - UTs + 2880) % 1440;
		setDist = (setDist > 720) ? 1440 - setDist : setDist;

		normT = (curT - UTr + 1440) % 1440;

		if (setDist < grayD / 2.0 || riseDist < grayD / 2.0)
			state = "gray";
		else if (normT > 0 && normT < dayLen)
			state = "day";
		else
			state = "night";

		if (!verbose)
			printf("%s\n", state);
		else {
			printf("CurT: %05.2f normT: %5.2f rDist: %05.2f sDist: %05.2f State: %s\n",
				curT / 60.0, normT / 60.0, riseDist / 60.0, setDist / 60.0, state);
			printf("Distance to sunrise: %.2f to sunset: %.2f\n", riseDist / 60.0, setDist / 60.0);
			printf("Sunrise: %5.2f Sunset: %5.2f\n", UTrH, UTsH);
		}
	}

	free(sYear);
	free(sMonth);
	free(sDay);
	free(sHour);
	free(sMinute);

	return 0;
}

