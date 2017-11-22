#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libserialport.h>

#include "Wheel.h"

#define WCTRL_HEAD_CHAR 'W'
#define WCTRL_BUFLEN    7

#define WCTRL_RESP_STR	"WOK"

#define WCTRL_MIN_SPEED 0
#define WCTRL_MAX_SPEED 510

int WCTRL_Init(WCTRL* wheelCtrlPtr, const char* deviceName, int baudrate)
{
    int iResult;

    struct sp_port* serialPort;

    // Get Port
    iResult = sp_get_port_by_name(deviceName, &serialPort);
    if(iResult != SP_OK)
    {
        return WCTRL_SERIAL_FAILED;
    }

    // Open Port
    iResult = sp_open(serialPort, SP_MODE_READ_WRITE);
    if(iResult != SP_OK)
    {
        sp_free_port(serialPort);
        return WCTRL_SERIAL_FAILED;
    }

    // Set Baudrate
    iResult = sp_set_baudrate(serialPort, baudrate);
    if(iResult != SP_OK)
    {
        sp_close(serialPort);
        sp_free_port(serialPort);
        return WCTRL_SERIAL_FAILED;
    }

    // Set Bits of Byte
    iResult = sp_set_bits(serialPort, 8);
    if(iResult != SP_OK)
    {
        sp_close(serialPort);
        sp_free_port(serialPort);
        return WCTRL_SERIAL_FAILED;
    }

    // Set serial port reference
    *wheelCtrlPtr = (WCTRL)serialPort;

    return WCTRL_NO_ERROR;
}

int WCTRL_Control(WCTRL wheelCtrl, int leftSpeed, int rightSpeed, int timeout)
{
    int iResult;
    char ctrlBuf[WCTRL_BUFLEN + 1] = {0};
    struct sp_port* serialPort = (struct sp_port*)wheelCtrl;

	int timeCost;
	struct timespec timeHold, tmpTime;

	char tmpRead;
	int respStrLen, counter;

    // Check if speed all legal
    if(leftSpeed < WCTRL_MIN_SPEED || leftSpeed > WCTRL_MAX_SPEED)
    {
        return WCTRL_ILLEGAL_ARGUMENT;
    }
    if(rightSpeed < WCTRL_MIN_SPEED || rightSpeed > WCTRL_MAX_SPEED)
    {
        return WCTRL_ILLEGAL_ARGUMENT;
    }

    // Process Control String
    sprintf(ctrlBuf, "%c%03d%03d", WCTRL_HEAD_CHAR, leftSpeed, rightSpeed);

	// Get current time
	clock_gettime(CLOCK_MONOTONIC, &timeHold);

    // Output Control String
    iResult = sp_blocking_write(serialPort, ctrlBuf, WCTRL_BUFLEN, timeout);
    if(iResult < 0)
    {
        return WCTRL_SERIAL_FAILED;
    }

	// Get response
	respStrLen = strlen(WCTRL_RESP_STR);
	counter = 0;
	while(1)
	{
		// Checking time cost
		clock_gettime(CLOCK_MONOTONIC, &tmpTime);
		tmpTime.tv_sec = tmpTime.tv_sec - timeHold.tv_sec;
		tmpTime.tv_nsec = tmpTime.tv_nsec - timeHold.tv_nsec;
		timeCost = tmpTime.tv_sec * 1000 + tmpTime.tv_nsec / 1000;
		if(timeCost >= timeout)
		{
			return WCTRL_TIMEOUT;
		}

		// Non-blocking reading
		iResult = sp_nonblocking_read(serialPort, &tmpRead, sizeof(char));
		if(iResult == 0)
		{
			continue;
		}
		else if(iResult < 0)
		{
			return WCTRL_SERIAL_FAILED;
		}

		// Check and store received data
		if(counter == 0 && tmpRead != WCTRL_HEAD_CHAR)
		{
			continue;
		}
		else
		{
			ctrlBuf[counter++] = tmpRead;
		}

		// Check if finished
		if(counter == respStrLen)
		{
			ctrlBuf[counter] = '\0';
			break;
		}
	}

	// Check response
	if(strcmp(ctrlBuf, WCTRL_RESP_STR) != 0)
	{
		return WCTRL_WRONG_RESPONSE;
	}

    return WCTRL_NO_ERROR;
}

int WCTRL_Close(WCTRL wheelCtrl)
{
    int iResult;
    struct sp_port* serialPort = (struct sp_port*)wheelCtrl;

    // Closing and Free Port
    iResult = sp_close(serialPort);
    sp_free_port(serialPort);

    if(iResult != SP_OK)
    {
        return WCTRL_SERIAL_FAILED;
    }

    return WCTRL_NO_ERROR;
}
