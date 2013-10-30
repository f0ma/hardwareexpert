#ifndef FTDIINTERFACEPROVIDER_H
#define FTDIINTERFACEPROVIDER_H

#include <QObject>
#include <QLibrary>
#include <QDebug>
#include <QFileInfo>
#include <QCoreApplication>
#include <QProcess>

#ifdef Q_WS_WIN
#include <windows.h>
#else
typedef unsigned int UINT;
typedef char* LPCTSTR;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned int* PDWORD;
typedef unsigned int* LPDWORD;
typedef void*	PVOID;
typedef void*	LPVOID;
typedef unsigned short USHORT;
#endif

typedef void*	FT_HANDLE;
typedef ULONG	FT_STATUS;

typedef struct {
    ULONG Flags;
    ULONG Type;
    ULONG ID;
    DWORD LocId;
    char SerialNumber[16];
    char Description[64];
    FT_HANDLE ftHandle;
} FT_DEVICE_LIST_INFO_NODE;



class FtdiInterfaceProvider
{

    QLibrary lib;
    bool initOk;
    QStringList serials;
    QList <FT_HANDLE> handles;
public:

static const USHORT FLOW_NONE=0x0000;
static const USHORT FLOW_RTS_CTS=0x0100;
static const USHORT FLOW_DTR_DSR=0x0200;
static const USHORT FLOW_XON_XOFF=0x0400;

static const USHORT PURGE_RX=1;
static const USHORT PURGE_TX=2;

#ifdef Q_WS_WIN
#define IMPORTED_FROM_LIBRARY __stdcall
#else
#define IMPORTED_FROM_LIBRARY 
#endif

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeCreateDeviceInfoList)(LPDWORD lpdwNumDevs);
    typedef FT_DEVICE_LIST_INFO_NODE * (IMPORTED_FROM_LIBRARY *PrototypeGetDeviceInfoList)(FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD lpdwNumDevs);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeOpenEx)(PVOID pArg1,DWORD Flags,FT_HANDLE *pHandle);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeClose)(FT_HANDLE ftHandle);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeRead)(FT_HANDLE ftHandle,LPVOID lpBuffer,DWORD dwBytesToRead,LPDWORD lpBytesReturned);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeWrite)(FT_HANDLE ftHandle, LPVOID lpBuffer,DWORD dwBytesToWrite,LPDWORD lpBytesWritten);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetBaudRate)(FT_HANDLE ftHandle,ULONG BaudRate);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetBitMode)(FT_HANDLE ftHandle,UCHAR ucMask,UCHAR ucEnable);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetUSBParameters)(FT_HANDLE ftHandle,ULONG ulInTransferSize,	ULONG ulOutTransferSize);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeGetQueueStatus)(FT_HANDLE ftHandle,DWORD *dwRxBytes);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetChars)(FT_HANDLE ftHandle,UCHAR EventChar,UCHAR EventCharEnabled,UCHAR ErrorChar,UCHAR ErrorCharEnabled);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypePurge)(FT_HANDLE ftHandle,ULONG Mask);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetTimeouts)(FT_HANDLE ftHandle,ULONG ReadTimeout,ULONG WriteTimeout);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetLatencyTimer)(FT_HANDLE ftHandle,UCHAR ucLatency);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetFlowControl)(FT_HANDLE ftHandle,USHORT FlowControl,UCHAR XonChar,UCHAR XoffChar);

    PrototypeCreateDeviceInfoList CreateDeviceInfoList;
    PrototypeGetDeviceInfoList GetDeviceInfoList;
    PrototypeOpenEx OpenEx;
    PrototypeClose Close;
    PrototypeRead Read;
    PrototypeWrite Write;
    PrototypeSetBaudRate SetBaudRate;
    PrototypeSetBitMode SetBitMode;
    PrototypeSetUSBParameters SetUSBParameters;
    PrototypeGetQueueStatus GetQueueStatus;
    PrototypeSetChars SetChars;
    PrototypePurge Purge;
    PrototypeSetTimeouts SetTimeouts;
    PrototypeSetLatencyTimer SetLatencyTimer;
    PrototypeSetFlowControl SetFlowControl;


    FtdiInterfaceProvider();
    int load();
    void unload();

    int getTotalPortsCount();

    void makeOperate();

    int getOperatePortsCount();
    FT_HANDLE getPortHandle(int no);
    QString getPortSerial(int no);

    void makeIdle();

    ~FtdiInterfaceProvider();
};

#endif // FTDIINTERFACEPROVIDER_H
