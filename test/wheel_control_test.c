#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <Wheel.h>

#define BASE_SPEED 		330
#define MIN_TIME_GAP	200

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
    int i;
    int iResult;
	int timeout;
	int baudrate;
	int leftSpeed, rightSpeed;
	int renew;
	char kbin;
	struct timespec timeHold, tmpTime;
    WCTRL wCtrl;

	// Check main args
	if(argc < 4)
	{
		printf("Use the following syntax to run Wheel test program:\n");
		printf("\twheel_control_test <Device_Path> <Device_Baudrate> <Timeout (ms)>\n\n");
		printf("Example: \n");
		printf("(Windows):\tWheel_test COM4 38400 20\n");
		printf("(Linux):\tWheel_test /dev/ttyS0 38400 0\n");
		printf("\n");

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
    iResult = WCTRL_Init(&wCtrl, argv[1], baudrate, STARTUP_TIMEOUT);
    if(iResult != WCTRL_NO_ERROR)
    {
		printf("Failed to open Wheel with device: %s, baudrate: %d\n", argv[1], baudrate);
        return -1;
    }

	// Manual controlling
	printf("Press WASD to test Wheel, or ESC to exit...\n");
	kbin = 0;
	renew = 0;
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	leftSpeed = 255;
	rightSpeed = 255;
	while(kbin != 27)
	{
		// Get keyboard input
		if(kbhit())
		{
			kbin = getch();
			clock_gettime(CLOCK_MONOTONIC, &timeHold);
			renew = 1;
		}

		// Find speeds
		clock_gettime(CLOCK_MONOTONIC, &tmpTime);
		if(get_diff_ms(&timeHold, &tmpTime) > MIN_TIME_GAP)
		{
			renew = 0;
		}

		if(renew == 0)
		{
			leftSpeed = 255;
			rightSpeed = 255;
		}
		else
		{
			switch(toupper(kbin))
			{
			case 'W':
				leftSpeed = BASE_SPEED;
				rightSpeed = BASE_SPEED;
				break;

			case 'A':
				leftSpeed = 510 - BASE_SPEED;
				rightSpeed = BASE_SPEED;
				break;

			case 'S':
				leftSpeed = 510 - BASE_SPEED;
				rightSpeed = 510 - BASE_SPEED;
				break;

			case 'D':
				leftSpeed = BASE_SPEED;
				rightSpeed = 510 - BASE_SPEED;
				break;

			default:
				leftSpeed = 255;
				rightSpeed = 255;
			}
		}

		// Controlling
		iResult = WCTRL_Control(wCtrl, leftSpeed, rightSpeed, timeout);
        if(iResult != WCTRL_NO_ERROR)
        {
			printf("Wheel control failed with error: %d\n", iResult);
            return -1;
        }
	}

	// Cleanup
    iResult = WCTRL_Close(wCtrl);
    if(iResult != WCTRL_NO_ERROR)
    {
		printf("Failed to close device!\n");
        return -1;
    }

    return 0;
}
