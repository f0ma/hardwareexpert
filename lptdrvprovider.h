#ifndef LPTDRVPROVIDER_H
#define LPTDRVPROVIDER_H

#include <QLibrary>

#ifdef Q_WS_WIN
#include <windows.h>
#else
typedef unsigned int UINT;
typedef char* LPCTSTR;
typedef unsigned int DWORD;
#endif

class LptDrvProvider
{

#ifdef Q_WS_WIN
    typedef UINT (__stdcall *PrototypeUintVoid)();
    typedef UINT (__stdcall *PrototypeUintDword)(DWORD A);
    typedef UINT (__stdcall *PrototypeUintUintDword)(UINT A,DWORD B);
    typedef UINT (__stdcall *PrototypeUintUint)(UINT A);
    typedef UINT (__stdcall *PrototypeUintDwordDword)(DWORD A,DWORD B);
    typedef UINT (__stdcall *PrototypeUintDwordDwordDword)(DWORD A,DWORD B,DWORD C);
    typedef UINT (__stdcall *PrototypeUintDwordDwordDwordDword)(DWORD A,DWORD B,DWORD C,DWORD D);
    typedef UINT (__stdcall *PrototypeUintDwordDwordDwordDwordDword)(DWORD A,DWORD B,DWORD C,DWORD D,DWORD E);

    typedef UINT (__stdcall *PrototypeUintStr)(LPCTSTR A);
    typedef UINT (__stdcall *PrototypeUintStrDword)(LPCTSTR A, DWORD B);
#else
         typedef UINT (*PrototypeUintVoid)();
         typedef UINT (*PrototypeUintDword)(DWORD A);
         typedef UINT (*PrototypeUintUintDword)(UINT A,DWORD B);
         typedef UINT (*PrototypeUintUint)(UINT A);
         typedef UINT (*PrototypeUintDwordDword)(DWORD A,DWORD B);
         typedef UINT (*PrototypeUintDwordDwordDword)(DWORD A,DWORD B,DWORD C);
         typedef UINT (*PrototypeUintDwordDwordDwordDword)(DWORD A,DWORD B,DWORD C,DWORD D);
         typedef UINT (*PrototypeUintDwordDwordDwordDwordDword)(DWORD A,DWORD B,DWORD C,DWORD D,DWORD E);

         typedef UINT (*PrototypeUintStr)(LPCTSTR A);
         typedef UINT (*PrototypeUintStrDword)(LPCTSTR A, DWORD B);
#endif

    QLibrary lib;

public:

    LptDrvProvider();

    bool load();

    PrototypeUintStr LptDrvCheckVersion;
    PrototypeUintVoid LptDrvClearError;
    PrototypeUintVoid LptDrvClose;
    PrototypeUintVoid LptDrvGetDllVersion;
    PrototypeUintVoid LptDrvGetError;
    PrototypeUintUint LptDrvGetErrorString;
    PrototypeUintVoid LptDrvGetNumPorts;
    PrototypeUintVoid LptDrvGetOpenedPort;
    PrototypeUintVoid LptDrvGetStatus;
    PrototypeUintVoid LptDrvGetVersion;

    PrototypeUintStrDword LptDrvGetVersionString;     //LptDrvGetVersionString is buggy & not need

    PrototypeUintUintDword LptDrvOpen;
    PrototypeUintVoid LptDrvReset;
    PrototypeUintDword LptDrvSetControl;
    PrototypeUintDword LptDrvSetData;
    PrototypeUintDword LptDrvSetDelay;
    PrototypeUintDword LptDrvSetTimeout;


    PrototypeUintVoid LptI2cCheckAdapter ;
    PrototypeUintDword LptI2cEnableAckCheck ;
    PrototypeUintDwordDword LptI2cOpen;
    PrototypeUintDwordDwordDwordDwordDword LptI2cRead;
    PrototypeUintDwordDword LptI2cReadByte;
    PrototypeUintDwordDwordDword LptI2cReadSubByte;
    PrototypeUintDwordDwordDword LptI2cReadSubWord;
    PrototypeUintDwordDword LptI2cReadWord;
    PrototypeUintDwordDwordDwordDwordDword LptI2cWrite;
    PrototypeUintDwordDword LptI2cWriteByte;
    PrototypeUintDwordDwordDword LptI2cWriteSubByte;
    PrototypeUintDwordDwordDword LptI2cWriteSubWord;
    PrototypeUintDwordDword LptI2cWriteWord;

    PrototypeUintDword LptSpiSetInvOut;
    PrototypeUintDwordDwordDword LptSpiWrite;
    PrototypeUintDword LptSpiWriteWord;
};

#endif // LPTDRVPROVIDER_H
