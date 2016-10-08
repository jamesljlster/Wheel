#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <Wheel.h>

#define BASE_SPEED 		330
#define MIN_TIME_GAP	200

int main(int argc, char* argv[])
{
    int i;
    int iResult;
	int ctrlDelay;
	int leftSpeed, rightSpeed;
	int renew;
	char kbin;
	time_t timeHold;
    WCTRL wCtrl;

	// Check main args
	if(argc < 4)
	{
		printf("Use the following syntax to run Wheel test program:\n");
		printf("\tWheel_test <Device_Path> <Device_Baudrate> <Control_Delay (ms)>\n\n");
		printf("Example: \n");
		printf("(Windows):\tWheel_test COM4 38400 20\n");
		printf("(Linux):\tWheel_test /dev/ttyS0 38400 0\n");
		printf("\n");

		return -1;
	}

    iResult = WCTRL_Init(&wCtrl, argv[1], atoi(argv[2]));
    if(iResult != WCTRL_NO_ERROR)
    {
		printf("Failed to open Wheel with device: %s, baudrate: %d\n", argv[1], atoi(argv[2]));
        return -1;
    }

	// Find control delay
	ctrlDelay = atoi(argv[3]);

	// Manual controlling
	printf("Press WASD to test Wheel, or ESC to exit...\n");
	kbin = 0;
	renew = 0;
	timeHold = clock();
	leftSpeed = 255;
	rightSpeed = 255;
	while(kbin != 27)
	{
		// Get keyboard input
		if(kbhit())
		{
			kbin = getch();
			timeHold = clock();
			renew = 1;
		}
		
		// Find speeds
		if(clock() - timeHold > MIN_TIME_GAP)
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
		iResult = WCTRL_Control(wCtrl, leftSpeed, rightSpeed, ctrlDelay);
        if(iResult != WCTRL_NO_ERROR)
        {
			printf("Wheel control failed!\n");
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
