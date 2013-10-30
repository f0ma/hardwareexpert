///////////////////////////////////////////////////////////////////////
//HardwareExpert. Class connector WinIO <-> LPT interface.
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

#include "ftdidioportinterface.h"
#include "settings.h"

bool FtdiDioPortInterface::isAvable()
{
    return this->load();
}

bool FtdiDioPortInterface::load()
{
int l = prov->load();

if (l==1) return true;

if (l==0) reasonText = QObject::tr("Can't find FTDI D2XX library!");

return false;
}

void FtdiDioPortInterface::unload()
{
prov->unload();
}

FtdiDioPortInterface::FtdiDioPortInterface(FtdiInterfaceProvider * provider)
{
	prov = provider;
}

QStringList FtdiDioPortInterface::getPortList()
{
    return QStringList();
}

void FtdiDioPortInterface::setPortName (QString n)
{
    Q_UNUSED(n);
}

QString FtdiDioPortInterface::getPortName()
{
    return "";
}

int FtdiDioPortInterface::getPortCount()
{
    return prov->getTotalPortsCount();
}

void FtdiDioPortInterface::setPortValue(int no,unsigned char value)
{
    if(no<ports.length())
    {
           DWORD readed =0;
           unsigned char data = value;
           prov->Write(prov->getPortHandle(no),&data,1,&readed);
           Q_ASSERT(readed==1);
    }
}

unsigned char FtdiDioPortInterface::getPortValue(int no)
{
    if(no<ports.length())
    {
           DWORD readed =0;
           unsigned char data = 0x00;
           prov->Read(prov->getPortHandle(no),&data,1,&readed);
           Q_ASSERT(readed==1);
           return data;
    }
    return 0;
}

void FtdiDioPortInterface::setPinMode(int no,int pin,IO_MODE mode)
{
    if(no<ports.length())
    {
    if(mode == IO_MODE_INPUT)
    {
        unsigned char mask = 0xFF;
        mask = mask - (1 << pin);
        bitmodes[no] = bitmodes[no]  & mask;
        prov->SetBitMode(prov->getPortHandle(no),bitmodes[no],0x04);
    }
    if(mode == IO_MODE_OUTPUT)
     {
        unsigned char mask = 0x0;
        mask = mask + (1 << pin);
        bitmodes[no] = bitmodes[no] | mask;
        prov->SetBitMode(prov->getPortHandle(no),bitmodes[no],0x04);

    }
    }
}

void FtdiDioPortInterface::setPortMode(int no,IO_MODE mode)
{
    if(no<prov->getOperatePortsCount())
    {
    if(mode == IO_MODE_INPUT)
    {
        prov->SetBitMode(prov->getPortHandle(no),0x00,0x00);
        prov->SetBitMode(prov->getPortHandle(no),0x00,0x01);
        prov->SetBaudRate(prov->getPortHandle(no),921600);
        bitmodes[no]=0;
    }
    if(mode == IO_MODE_OUTPUT)
     {
        prov->SetBitMode(prov->getPortHandle(no),0x00,0x00);
        prov->SetBitMode(prov->getPortHandle(no),0xFF,0x01);
        prov->SetBaudRate(prov->getPortHandle(no),921600);
        bitmodes[no]=0xFF;
    }
    }
}

bool FtdiDioPortInterface::open()
{
    prov->makeOperate();
    bitmodes.clear();
    int pcount = prov->getOperatePortsCount();
    for(int i =0;i<pcount;i++) bitmodes.append(0x00);
    return  pcount > 0;

}

void FtdiDioPortInterface::close()
{
    prov->makeIdle();
}



