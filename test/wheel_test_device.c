#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Wheel.h>

int main(int argc, char* argv[])
{
	int iResult;
	WCTRL wCtrl;

	// Checking
	if(argc < 3)
	{
		printf("Use the following syntax to run the test program\n");
		printf("    wheel_test_device <device_path> <baudrate>\n");
		return -1;
	}

	// Open device
	iResult = WCTRL_Init(&wCtrl, argv[1], atoi(argv[2]));
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Init() failed with device path: %s, baudrate: %d\n", argv[1], atoi(argv[2]));
		return -1;
	}

	// Test control
	iResult = WCTRL_Control(wCtrl, 300, 300, 20);
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Control() failed!\n");
		WCTRL_Close(wCtrl);
		return -1;
	}

	usleep(500 * 1000);

	iResult = WCTRL_Control(wCtrl, 255, 255, 20);
	if(iResult != WCTRL_NO_ERROR)
	{
		printf("WCTRL_Control() failed!\n");
		WCTRL_Close(wCtrl);
		return -1;
	}

	// Cleanup
	WCTRL_Close(wCtrl);

	return 0;
}
