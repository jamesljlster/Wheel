#ifndef WHEEL_H_INCLUDED
#define WHEEL_H_INCLUDED

enum WCTRL_RETURN_VALUE
{
    WCTRL_NO_ERROR          = 0,
    WCTRL_SERIAL_FAILED     = -1,
    WCTRL_ILLEGAL_ARGUMENT  = -2
};

typedef void* WCTRL;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int WCTRL_Init(WCTRL* wheelCtrlPtr, const char* deviceName, int baudrate);
int WCTRL_Control(WCTRL wheelCtrl, int leftSpeed, int rightSpeed, int delay);
int WCTRL_Close(WCTRL wheelCtrl);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WHEEL_H_INCLUDED
