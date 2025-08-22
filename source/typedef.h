#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef unsigned char bool;


typedef int(*UartSendFunc)(unsigned char *data, int len);
typedef void(*IoControlFunc)(unsigned char state);
typedef void(*PwmControlFunc)(unsigned char per);
typedef unsigned char(*IoInputFunc)(void);


typedef void(*KeyChangedFunc)(unsigned char state);
typedef void(*KeyPressedFunc)(void);
typedef void(*CmdDealFunc)(unsigned char *data, int len);

#endif
