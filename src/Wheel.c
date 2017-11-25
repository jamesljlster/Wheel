#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libserialport.h>

#include "Wheel.h"
#include "debug.h"

#define WCTRL_HEAD_CHAR 'W'
#define WCTRL_BUFLEN    7

#define WCTRL_RESP_STR	"WOK"
#define DEFAULT_TIMEOUT	200

#define WCTRL_MIN_SPEED 0
#define WCTRL_MAX_SPEED 510

int WCTRL_Init(WCTRL* wheelCtrlPtr, const char* deviceName, int baudrate, int timeout)
{
	int iResult;
	int respStrLen;
	char ctrlBuf[WCTRL_BUFLEN + 1] = {0};

	struct sp_port* serialPort;

	LOG("Using device: %s, baudrate: %d", deviceName, baudrate);

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

	// Get response
	respStrLen = strlen(WCTRL_RESP_STR);
	iResult = sp_blocking_read(serialPort, ctrlBuf, respStrLen, timeout);
	if(iResult > 0)
	{
		ctrlBuf[iResult] = '\0';
	}

	LOG("Receive: %s", ctrlBuf);
	if(iResult != respStrLen)
	{
		LOG("Serial read timeout!");
		return WCTRL_TIMEOUT;
	}

	// Check response
	if(strcmp(ctrlBuf, WCTRL_RESP_STR) != 0)
	{
		return WCTRL_WRONG_RESPONSE;
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

	int timeLeft;
	struct timespec timeHold, tmpTime;

	int respStrLen;

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
	clock_gettime(CLOCK_MONOTONIC, &tmpTime);
	tmpTime.tv_sec = tmpTime.tv_sec - timeHold.tv_sec;
	tmpTime.tv_nsec = tmpTime.tv_nsec - timeHold.tv_nsec;
	timeLeft = timeout - (tmpTime.tv_sec * 1000 + tmpTime.tv_nsec / 1000000);
	LOG("timeLeft = %d", timeLeft);
	if(timeLeft <= 0)
	{
		return WCTRL_TIMEOUT;
	}

	iResult = sp_blocking_read(serialPort, ctrlBuf, respStrLen, timeLeft);
	if(iResult > 0)
	{
		ctrlBuf[iResult] = '\0';
	}

	LOG("Receive: %s", ctrlBuf);
	if(iResult != respStrLen)
	{
		LOG("Serial read timeout!");
		return WCTRL_TIMEOUT;
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
