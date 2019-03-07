#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
	int rs; // Status from sunset/sunrise calculation
	int verbose = 0;
	time_t rawtime;
	struct tm *info;

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

	time(&rawtime);
	info = gmtime(&rawtime);
	year = info->tm_year + 1900;
	month = info->tm_mon + 1;
	day = info->tm_mday;
	curT = info->tm_hour * 60 + info->tm_min;

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
	}

	latitude = (grid[1] - 65) * 10 + (grid[3] - 48) + ((double)grid[5] - 65.) / 24 + 1. / 48 - 90;
	longitude = (grid[0] - 65) * 20 + (grid[2] - 48) * 2 + ((double)grid[4] - 65.) / 12 + 1. / 24 - 180;

	rs = sun_rise_set(year, month, day, longitude, latitude, &UTrH, &UTsH);

	UTs = round(60.0 * UTsH);
	UTr = round(60.0 * UTrH);

	if (verbose) {
		printf("Date: %4d-%02d-%02d Time: %02d:%02d ", year, month, day,
			(int)(curT / 60.0), (int)round(60 * (curT / 60.0 - floor(curT / 60.0))));
		printf("Twilight duration: %.2fh\n", grayD / 60.0);
		printf("Grid locator: %s ==> ", grid);
		printf("Latitude = %.4f deg. Longitude = %.4f deg.\n", latitude, longitude);
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
		if (verbose)
			printf("Normal day/night cycle.\n");

		dayLen = (UTs - UTr + 1440) % 1440;

		// Calculate distance to sunrise
		riseDist = (int)(curT - UTr + 2880) % 1440;
		riseDist = (riseDist > 720) ? 1440 - riseDist : riseDist;

		// Calculate distance to sunset
		setDist =  (int)(curT - UTs + 2880) % 1440;
		setDist = (setDist > 720) ? 1440 - setDist : setDist;

		// Calculate current time relative to sunrise
		normT = (curT - UTr + 1440) % 1440; 

		if (setDist < grayD / 2.0 || riseDist < grayD / 2.0)
			// We are within grayD/2 of sunset or sunrise
			state = "gray";
		else if (normT > 0 && normT < dayLen)
			// We are after sunrise and before sunset
			state = "day";
		else
			// Else its night
			state = "night";

		if (!verbose)
			printf("%s\n", state);
		else {
//			printf("CurT: %05.2f normT: %5.2f rDist: %05.2f sDist: %05.2f State: %s\n",
//				curT / 60.0, normT / 60.0, riseDist / 60.0, setDist / 60.0, state);
			printf("Sunrise: %02d:%02d Sunset: %02d:%02d\n", 
				(int)(UTrH), (int)round(60 * (UTrH - floor(UTrH))),
				(int)(UTsH), (int)round(60 * (UTsH - floor(UTsH))));
			printf("Time from sunrise: %.2fh from sunset: %.2fh\nSolar state: %s\n", 
				riseDist / 60.0, setDist / 60.0, state);
		}
	}

	free(sYear);
	free(sMonth);
	free(sDay);
	free(sHour);
	free(sMinute);

	return 0;
}

