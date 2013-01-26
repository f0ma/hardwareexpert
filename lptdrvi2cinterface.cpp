///////////////////////////////////////////////////////////////////////
//HardwareExpert. Class connector LptDrv <-> I2C interface.
//Copyright (C) 2011  Stanislav (f0ma) Ivanov
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////

#include "lptdrvi2cinterface.h"

LptDrvI2cInterface::LptDrvI2cInterface()
{
err=0;
}

bool LptDrvI2cInterface::load()
{
    if (prov.load()) return true;

    reasonText = QObject::tr("Can't load LptDrv library or driver!");
    return false;
}

void LptDrvI2cInterface::unload()
{
     prov.LptDrvClose();
}

void LptDrvI2cInterface::setPortName(QString n)
{
    Q_UNUSED(n); //\warning Hardcoded there
}

QString LptDrvI2cInterface::getPortName()
{
    return "LPT1"; //\warning Hardcoded there
}

QStringList LptDrvI2cInterface::getPortList()
{
    QStringList r;
    r<< "LPT1";
    return r;
}

bool LptDrvI2cInterface::open()
{
#ifdef Q_WS_WIN

    //int LPTDelay  = 10;


    if (!prov.load()) return false;

    prov.LptDrvGetDllVersion();
    prov.LptDrvOpen(1, 0x60); //\warning Hardcoded there
    prov.LptDrvGetVersion();
    prov.LptDrvGetNumPorts();
    prov.LptDrvSetData(0xc0);
    prov.LptDrvSetControl(0x0f);
    prov.LptDrvClose();
    prov.LptDrvOpen(1, 0x70);
    prov.LptDrvSetData(0xc0);
    prov.LptDrvSetControl(0x0f);
    prov.LptDrvSetDelay(10);



    prov.LptDrvClose();

    //QWaitCondition sleep;
    //QMutex s;
    //sleep.wait(&s,100);

    if((unsigned int)prov.LptDrvOpen(1,0x8c) == 0)
    {
        return false;
    }//\warning Hardcoded there
    prov.LptDrvSetDelay(10);


    return true;
#endif

    return false;
}

void LptDrvI2cInterface::close()
{
#ifdef Q_WS_WIN
    prov.LptDrvClearError();
    prov.LptDrvClose();
#endif

    inputBuffer.clear();
    operations.clear();
}

void LptDrvI2cInterface::addWrite (unsigned char adr,QByteArray data)
{
    operation n;
    n.type=operation::WRITE;
    n.adr=adr;
    n.data=data;
    operations.append(n);
}

void LptDrvI2cInterface::addRead (unsigned char adr,unsigned int size)
{
    operation n;
    n.type=operation::READ;
    n.adr=adr;
    n.size=size;
    operations.append(n);
}

void LptDrvI2cInterface::addStop()
{
    operation n;
    n.type=operation::STOP;
    operations.append(n);
}


void LptDrvI2cInterface::checkAsk(bool a)
{
#ifdef Q_WS_WIN
   if(a == false) prov.LptI2cEnableAckCheck(0);
   else prov.LptI2cEnableAckCheck(1);
#else
   Q_UNUSED(a);
#endif
}

void LptDrvI2cInterface::writeNow (unsigned char adr,QByteArray data)
{
    unsigned char * d = new unsigned char [data.size()];
    for(int i=0;i<data.size();i++) d[i]=data[i];

#ifdef Q_WS_WIN
    err=prov.LptI2cWrite((UINT)(adr>>1), (UINT)d, data.size(),0, 0);
#else
    Q_UNUSED(adr);
#endif
    delete [] d;
}

QByteArray LptDrvI2cInterface::readNow (unsigned char adr,unsigned int size)
{
    unsigned char * d = new unsigned char [size];
    for(unsigned int i=0;i<size;i++) d[i]=0;

#ifdef Q_WS_WIN
    err=prov.LptI2cRead((UINT)(adr>>1),0, 0, (UINT) d, (UINT)size);
#else
    Q_UNUSED(adr);
#endif

    QByteArray r;
    r.append((const char*)d,size);

    delete [] d;

    return r;
}

QByteArray LptDrvI2cInterface::writeReadNow (unsigned char adr,QByteArray data ,unsigned int size)
{
    unsigned char * w = new unsigned char [data.size()];
    for(int i=0;i<data.size();i++) w[i]=data[i];

    unsigned char * r = new unsigned char [size];
    for(unsigned int i=0;i<size;i++) r[i]=0;


#ifdef Q_WS_WIN
    err=prov.LptI2cRead((UINT)(adr>>1),(UINT) w, data.size(), (UINT) r, size);
#else
    Q_UNUSED(adr);
#endif

    QByteArray rt;
    rt.append((const char*)r,size);

delete [] w;
delete [] r;

    return rt;

}

bool LptDrvI2cInterface::isOk()
{
    if(err==0)return true;
    return false;
}


bool LptDrvI2cInterface::preform()
{
    operation p;

    QString s;
;

    foreach(p,operations)
    {
        if(p.type==operation::WRITE)
        {
            unsigned char * d = new unsigned char [p.data.size()];
            for(int i=0;i<p.data.size();i++) d[i]=p.data[i];

#ifdef Q_WS_WIN

            //prov.LptDrvClearError();
            err=prov.LptI2cWrite((UINT)(p.adr>>1), 0, 0, (UINT)d, p.data.size());
#endif
            delete [] d;
        }

        if(p.type==operation::READ)
        {
            unsigned char * d = new unsigned char [p.size];
            for(unsigned int i=0;i<p.size;i++) d[i]=0;

#ifdef Q_WS_WIN
            //prov.LptDrvClearError();
            err=prov.LptI2cRead((UINT)(p.adr>>1),0, 0, (UINT) d, p.size);
#endif
                inputBuffer.append((const char*)d,p.size);
            delete [] d;
        }

        if(p.type==operation::STOP)
        {
#ifdef Q_WS_WIN
            prov.LptDrvClearError();
#endif
        }

        if(p.type==operation::WAIT)
        {
            QWaitCondition sleep;
            QMutex s;
            sleep.wait(&s,p.time);
        }
    }

    return true;
}

void LptDrvI2cInterface::addWait(int time)
{
    operation n;
    n.type=operation::WAIT;
    n.time=time;
    operations.append(n);
}

int LptDrvI2cInterface::getBufferLength()
{
    return inputBuffer.length();
}

QByteArray LptDrvI2cInterface::getBuffer()
{
    QByteArray r=inputBuffer;

    inputBuffer.clear();

    return r;
}
