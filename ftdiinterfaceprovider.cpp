#include "ftdiinterfaceprovider.h"

FtdiInterfaceProvider::FtdiInterfaceProvider()
{
#ifdef Q_WS_WIN
    lib.setFileName("ftd2xx.dll");
#else
    lib.setFileName("./libftd2xx");
#endif
    initOk = false;
}

int FtdiInterfaceProvider::load()
{
    if(initOk) return 1;

#ifndef Q_WS_WIN
    QProcess process;
    process.start("python", QStringList() << (QFileInfo( QCoreApplication::applicationFilePath() ).filePath()+"/grant_full_access_to_ftdi_devices.py"));
    process.waitForFinished ( -1 );
#endif

    lib.load();

    if(!lib.isLoaded())
    {
        initOk = false;
        qDebug() << lib.errorString();
        return 0;
    }

    CreateDeviceInfoList = (PrototypeCreateDeviceInfoList) lib.resolve("FT_CreateDeviceInfoList");
    GetDeviceInfoList = (PrototypeGetDeviceInfoList) lib.resolve("FT_GetDeviceInfoList");
    OpenEx = (PrototypeOpenEx) lib.resolve("FT_OpenEx");
    Close = (PrototypeClose) lib.resolve("FT_Close");
    Read = (PrototypeRead) lib.resolve("FT_Read");
    Write = (PrototypeWrite) lib.resolve("FT_Write");
    SetBaudRate = (PrototypeSetBaudRate) lib.resolve("FT_SetBaudRate");
    SetBitMode = (PrototypeSetBitMode) lib.resolve("FT_SetBitMode");
    SetUSBParameters = (PrototypeSetUSBParameters) lib.resolve("FT_SetUSBParameters");
    GetQueueStatus = (PrototypeGetQueueStatus) lib.resolve("FT_GetQueueStatus");
    SetChars = (PrototypeSetChars) lib.resolve("FT_SetChars");
    Purge = (PrototypePurge) lib.resolve("FT_Purge");
    SetTimeouts = (PrototypeSetTimeouts) lib.resolve("FT_SetTimeouts");
    SetLatencyTimer = (PrototypeSetLatencyTimer) lib.resolve("FT_SetLatencyTimer");
    SetFlowControl = (PrototypeSetFlowControl) lib.resolve("FT_SetFlowControl");

     return 1;
}

void FtdiInterfaceProvider::unload()
{

}

FtdiInterfaceProvider::~FtdiInterfaceProvider()
{

}
