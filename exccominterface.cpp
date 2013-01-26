///////////////////////////////////////////////////////////////////////
//HardwareExpert. Class connector QextSerialPort <-> COM interface.
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

#include "exccominterface.h"

ExcComInterface::ExcComInterface()
{
    port = NULL;
}

ExcComInterface::~ExcComInterface()
{
    if(port!=NULL) delete port;
}


void ExcComInterface::setPortName(QString n)
{
    portName=n;
}

bool ExcComInterface::open (int baudrate,int flowContol, int partity,int dataBits, int stopBits)
{
    if(port!=NULL) delete port;

    port = new QextSerialPort(portName);

    port->setBaudRate((BaudRateType)baudrate);
    port->setFlowControl((FlowType)flowContol);
    port->setParity((ParityType)partity);
    port->setDataBits((DataBitsType)dataBits);
    port->setStopBits((StopBitsType)stopBits);
#ifdef Q_OS_LINUX
    port->setTimeout(0,0);
#else
    port->setTimeout(0);
#endif
    
     port->open(QIODevice::ReadWrite);

     if(!port->isOpen())
     {
         delete port;
         port = NULL;
         return false;
     }

     return true;

}

void ExcComInterface::close ()
{
    if(port!=NULL) delete port;
    port=NULL;
}

void ExcComInterface::write (QByteArray data)
{
    if(port!=NULL)
    {
        port->write(data);
    }
}

QByteArray ExcComInterface::read ()
{
    if(port!=NULL)
    {
        return port->readAll();
    }
    return "";
}

int ExcComInterface::bytesAvailable ()
{
    if(port!=NULL)
    {
        return port->bytesAvailable();
    }
    return 0;
}

void ExcComInterface::setDTR (bool e)
{
    if(port!=NULL) port->setDtr(e);
}

void ExcComInterface::setRTS (bool e)
{
    if(port!=NULL) port->setRts(e);
}

QString ExcComInterface::getPortName()
{
    return portName;
}

QStringList ExcComInterface::getPortList()
{
    QStringList ports;

#ifdef Q_OS_LINUX
    ports << "/dev/ttyS0"
          << "/dev/ttyS1"
          << "/dev/ttyUSB0"
          << "/dev/ttyUSB1"
          << "";
#else
    ports << "COM1"
          << "COM2"
          << "COM3"
          << "COM4"
          << "COM5"
          << "COM6"
          << "COM7"
          << "COM8"
          << "COM9"
          << "COM10"
          << "COM11"
          << "COM12"
          << "COM13"
          << "COM14"
          << "COM15"
          << "COM16"
          << "";
#endif

    return ports;
}

void ExcComInterface::setTimeout(int v)
{
#ifdef Q_OS_LINUX
    if(port!=NULL) port->setTimeout(v,0);
#else
    if(port!=NULL) port->setTimeout(v);
#endif
}
