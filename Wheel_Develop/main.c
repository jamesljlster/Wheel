#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#include "Wheel.h"

int main()
{
    int i;
    int iResult;
    WCTRL wCtrl;

    iResult = WCTRL_Init(&wCtrl, "COM3", 38400);
    if(iResult != WCTRL_NO_ERROR)
    {
        return -1;
    }

    for(i = 0; i < 255; i++)
    {
        iResult = WCTRL_Control(wCtrl, i, 510 - i);
        if(iResult != WCTRL_NO_ERROR)
        {
            return -1;
        }

        Sleep(100);
    }

    iResult = WCTRL_Close(wCtrl);
    if(iResult != WCTRL_NO_ERROR)
    {
        return -1;
    }

    return 0;
}
