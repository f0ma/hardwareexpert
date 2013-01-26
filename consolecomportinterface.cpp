///////////////////////////////////////////////////////////////////////
//HardwareExpert. Print only interface for COM port.
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

#include "consolecomportinterface.h"

ConsoleComPortInterface::ConsoleComPortInterface()
{
}

ConsoleComPortInterface::~ConsoleComPortInterface()
{

}

void ConsoleComPortInterface::setPortName(QString n)
{
    Q_UNUSED(n);
}

QString ConsoleComPortInterface::getPortName()
{
    return "";
}

QStringList ConsoleComPortInterface::getPortList()
{
    return QStringList();
}

void ConsoleComPortInterface::setConsole(ConsoleInterface * acon)
{
    con=acon;
}

bool ConsoleComPortInterface::open (int baudrate,int flowContol, int partity,int dataBits, int stopBits)
{
    Q_UNUSED(baudrate);
    Q_UNUSED(flowContol);
    Q_UNUSED(partity);
    Q_UNUSED(dataBits);
    Q_UNUSED(stopBits);

    con->print(QObject::tr("Port opened.\n"));
    return true;
}

void ConsoleComPortInterface::close ()
{
    con->print(QObject::tr("Port closed.\n"));
}

void ConsoleComPortInterface::write (QByteArray data)
{
    con->print(QObject::tr("Write to port:")+data.toHex()+"\n");
}

QByteArray ConsoleComPortInterface::read ()
{
    con->print(QObject::tr("Read from port\n"));
    return "";
}

int ConsoleComPortInterface::bytesAvailable ()
{
    con->print(QObject::tr("Get Byte Count\n"));
    return 0;
}

void ConsoleComPortInterface::setDTR (bool e)
{
    QString s;
    s.setNum(e);
    con->print(QObject::tr("Set DTR to:") + s);
}

void ConsoleComPortInterface::setRTS (bool e)
{
    QString s;
    s.setNum(e);
    con->print(QObject::tr("Set RTS to:") + s);
}

void ConsoleComPortInterface::setTimeout (int v)
{
    QString s;
    s.setNum(v);
    con->print(QObject::tr("Set Timeout:") + s + QObject::tr(" ms"));
}
