#ifndef WINIOPROVIDER_H
#define WINIOPROVIDER_H

#include <QObject>
#include <QLibrary>

#ifdef Q_WS_WIN
#define _WIN32_WINNT 0x501
#include <windows.h>
#else
typedef unsigned int UINT;
typedef char* LPCTSTR;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int* PDWORD;
#endif

class WinIOProvider
{
#ifdef Q_WS_WIN
    typedef bool (__stdcall *PrototypeBoolVoid)();
    typedef void (__stdcall *PrototypeVoidVoid)();

    typedef bool (__stdcall *PrototypeWordPdwordByte)(WORD A, PDWORD B, BYTE C);
    typedef bool (__stdcall *PrototypeWordDwordByte)(WORD A, DWORD B, BYTE C);
#else
    typedef bool (*PrototypeBoolVoid)();
    typedef void (*PrototypeVoidVoid)();

    typedef bool (*PrototypeWordPdwordByte)(WORD A, PDWORD B, BYTE C);
    typedef bool (*PrototypeWordDwordByte)(WORD A, DWORD B, BYTE C);
#endif

    QLibrary lib;
    bool initOk;

    PrototypeBoolVoid InitializeWinIo;
    PrototypeVoidVoid ShutdownWinIo;
    PrototypeWordPdwordByte GetPortVal;
    PrototypeWordDwordByte SetPortVal;

public:
    WinIOProvider();
    int load();
    void unload();
    void setPort(unsigned int port,unsigned char value);
    unsigned char getPort(unsigned int port);
    ~WinIOProvider();
};

#endif // WINIOPROVIDER_H
