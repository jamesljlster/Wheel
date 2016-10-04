#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Wheel.h>

int main()
{
    int i;
    int iResult;
    WCTRL wCtrl;

    iResult = WCTRL_Init(&wCtrl, "COM4", 38400);
    if(iResult != WCTRL_NO_ERROR)
    {
        return -1;
    }

    for(i = 0; i <= 70; i++)
    {
        if(kbhit())
        {
            if(getch() == 27)
            {
                break;
            }
        }

        printf("Send: %d, %d\n", i, 510 - i);
        iResult = WCTRL_Control(wCtrl, i, 510 - i, 10);
        if(iResult != WCTRL_NO_ERROR)
        {
            return -1;
        }
        usleep(100000);
    }

    iResult = WCTRL_Close(wCtrl);
    if(iResult != WCTRL_NO_ERROR)
    {
        return -1;
    }

    return 0;
}
