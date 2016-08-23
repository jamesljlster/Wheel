#include <stdio.h>
#include <unistd.h>
#include <libserialport.h>

#include "Wheel.h"

#define WCTRL_HEAD_CHAR 'W'
#define WCTRL_BUFLEN    7

#define WCTRL_MIN_SPEED 0
#define WCTRL_MAX_SPEED 510

#define WCTRL_WRITE_WATIING 100

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

int WCTRL_Control(WCTRL wheelCtrl, int leftSpeed, int rightSpeed, int delay)
{
    int iResult;
    char ctrlBuf[WCTRL_BUFLEN + 1] = {0};
    struct sp_port* serialPort = (struct sp_port*)wheelCtrl;

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

    // Output Control String
    iResult = sp_blocking_write(serialPort, ctrlBuf, WCTRL_BUFLEN, WCTRL_WRITE_WATIING);
    if(iResult < 0)
    {
        return WCTRL_SERIAL_FAILED;
    }

    // Delay
    usleep(delay * 1000);

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
