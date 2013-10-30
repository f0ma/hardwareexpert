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
    QString ds= QCoreApplication::applicationDirPath ();
    ds =ds.replace("/hardwareexpert","");
    process.start("python", QStringList() << ( ds+"/grant_full_access_to_ftdi_devices.py"));
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
	makeIdle();
}

FtdiInterfaceProvider::~FtdiInterfaceProvider()
{
	makeIdle();
}

int FtdiInterfaceProvider::getTotalPortsCount()
{
if(this->load()!=1) return 0;
DWORD numDevs;
if (this->CreateDeviceInfoList (&numDevs) == 0 && numDevs >0)
return numDevs;
else return 0; 
}

void FtdiInterfaceProvider::makeOperate()
{
    DWORD numDevs;

    if(this->load()!=1) return;

    if (this->CreateDeviceInfoList (&numDevs) == 0 && numDevs >0)
    {
        FT_DEVICE_LIST_INFO_NODE * devInfo = (FT_DEVICE_LIST_INFO_NODE*) malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        Q_ASSERT(this->GetDeviceInfoList(devInfo,&numDevs)==0);

	QStringList allSerials;

        for(unsigned int i = 0;i<numDevs;i++)
        {
        allSerials.append(devInfo[i].SerialNumber);
        }


        QString serial;
        handles.clear();
        serials.clear();

        if(allSerials.length()==0)return;

        foreach (serial,allSerials)
        {
            FT_HANDLE ftH=0;
            FT_STATUS ftStatus=0;

            ftStatus = this->OpenEx((void *)serial.toLocal8Bit().constData(),1,&ftH);
            ftStatus = this->SetBitMode(ftH,0x00,0x00);
            ftStatus = this->SetBitMode(ftH,0x00,0x01);
            ftStatus = this->SetBaudRate(ftH,921600);

            qDebug()<< serial<< ftStatus;
            handles.append(ftH);
	    serials.append(serial);
        }
	}

}

int FtdiInterfaceProvider::getOperatePortsCount()
{
	return handles.length();
}

FT_HANDLE FtdiInterfaceProvider::getPortHandle(int no)
{
	return handles[no];
}

QString FtdiInterfaceProvider::getPortSerial(int no)
{
	return serials[no];
}

void FtdiInterfaceProvider::makeIdle()
{
    FT_HANDLE ftH;
    foreach (ftH,handles)
    {
        this->Close(ftH);
    }

    handles.clear();
}
