#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <Wheel.h>

#define STARTUP_TIMEOUT	2000

long int get_diff_ms(struct timespec* start, struct timespec* end)
{
	struct timespec tmpTime;

	tmpTime.tv_sec = end->tv_sec - start->tv_sec;
	tmpTime.tv_nsec = end->tv_nsec - start->tv_nsec;

	return tmpTime.tv_sec * 1000 + tmpTime.tv_nsec / 1000000;
}

int main(int argc, char* argv[])
{
	int iResult;
	int timeout;
	int baudrate;
	int sal, sar;
	WCTRL wCtrl;

	struct timespec timeHold, tmpTime;
	clock_t timeCost;

	// Checking
	if(argc < 4)
	{
		printf("Use the following syntax to run the test program\n");
		printf("    wheel_test_device <device_path> <baudrate> <timeout>\n");
		return -1;
	}

	// Parse argument
	baudrate = atoi(argv[2]);
	timeout = atoi(argv[3]);

	// Print summary
	printf("Using device path: %s\n", argv[1]);
	printf("Using baudrate: %d\n", baudrate);
	printf("Using timeout: %d\n", timeout);
	printf("\n");

	// Open device
	printf("Initialize device with path: %s, baudrate: %d\n", argv[1], baudrate);
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	iResult = WCTRL_Init(&wCtrl, argv[1], atoi(argv[2]), STARTUP_TIMEOUT);
	clock_gettime(CLOCK_MONOTONIC, &tmpTime);
	timeCost = get_diff_ms(&timeHold, &tmpTime);
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Init() failed with error: %d, time cost: %ld ms\n", iResult, timeCost);
		return -1;
	}
	else
	{
		printf("Succeed, time cost: %ld ms\n", timeCost);
	}
	printf("\n");

	// Test control
	sal = 300;
	sar = 300;
	printf("Test device with SAL: %d, SAR: %d\n", sal, sar);
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	iResult = WCTRL_Control(wCtrl, sal, sar, timeout);
	clock_gettime(CLOCK_MONOTONIC, &tmpTime);
	timeCost = get_diff_ms(&timeHold, &tmpTime);
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Control() failed with error: %d, cost %ld ms\n", iResult, timeCost);
	}
	else
	{
		printf("Succeed, cost %ld ms\n", timeCost);
	}
	printf("\n");

	usleep(500 * 1000);

	sal = 300;
	sar = 300;
	printf("Test device with SAL: %d, SAR: %d\n", sal, sar);
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	iResult = WCTRL_Control(wCtrl, 255, 255, timeout);
	clock_gettime(CLOCK_MONOTONIC, &tmpTime);
	timeCost = get_diff_ms(&timeHold, &tmpTime);
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Control() failed with error: %d, cost %ld ms\n", iResult, timeCost);
	}
	else
	{
		printf("Succeed, cost %ld ms\n", timeCost);
	}
	printf("\n");

	// Cleanup
	WCTRL_Close(wCtrl);

	return 0;
}
