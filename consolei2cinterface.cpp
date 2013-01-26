///////////////////////////////////////////////////////////////////////
//HardwareExpert. Print only interface for I2C port.
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

#include "consolei2cinterface.h"

ConsoleI2cInterface::ConsoleI2cInterface()
{
    con=NULL;
}

void ConsoleI2cInterface::setConsole(ConsoleInterface * acon)
{
    con=acon;
}

bool ConsoleI2cInterface::open()
{
    return true;
}

void ConsoleI2cInterface::close()
{
    inputBuffer.clear();
    operations.clear();
}

void ConsoleI2cInterface::addWrite (unsigned char adr,QByteArray data)
{
    operation n;
    n.type=operation::WRITE;
    n.adr=adr;
    n.data=data;
    operations.append(n);
}

void ConsoleI2cInterface::addRead (unsigned char adr,unsigned int size)
{
    operation n;
    n.type=operation::READ;
    n.adr=adr;
    n.size=size;
    operations.append(n);
}

void ConsoleI2cInterface::addStop()
{
    operation n;
    n.type=operation::STOP;
    operations.append(n);
}

bool ConsoleI2cInterface::preform()
{
    operation p;

    QString s;

    con->print("\nI2C:");

    if(operations.count()==0)
    {
        con->print(" NOP \n");
        return true;
    }

    foreach(p,operations)
    {
        if(p.type==operation::WRITE)
        {

        s.setNum(p.adr & 0xfe,16);
        if(s.length()==1)s="0"+s;
        con->print(" S "+s);

        s =p.data.toHex().toUpper();

        int v = s.size()+s.size()/2;
        for(int i=0;i<v;i+=2)
        {
            s.insert(i," ");
            i++;
        }

        con->print(s+" ");
        }

        if(p.type==operation::READ)
        {

        s.setNum(p.adr | 0x01,16);
        if(s.length()==1)s="0"+s;
        con->print(" S "+s);
        s.setNum(p.size);
        con->print(QObject::tr(" [Reading: ")+ s + QObject::tr(" bytes] "));
        QByteArray v;
        v.fill(0x00,p.size);
        inputBuffer.append(v);
        }

        if(p.type==operation::STOP)
        {
            con->print("P\n");
        }

        if(p.type==operation::WAIT)
        {
            s.setNum(p.time);
            con->print(QObject::tr(" [Wating ") +s+ QObject::tr("ms] "));
        }
    }

    operations.clear();

    return true;
}

void ConsoleI2cInterface::addWait(int time)
{
    operation n;
    n.type=operation::WAIT;
    n.time=time;
    operations.append(n);
}

int ConsoleI2cInterface::getBufferLength()
{
    return inputBuffer.length();
}

QByteArray ConsoleI2cInterface::getBuffer()
{
    QByteArray r;
    r.append((QByteArray)inputBuffer);

    inputBuffer.clear();

    return r;
}

void ConsoleI2cInterface::checkAsk(bool a)
{
    Q_UNUSED(a);
}

void ConsoleI2cInterface::writeNow (unsigned char adr,QByteArray data)
{
    addWrite (adr,data);
    addStop();
    preform();
}

QByteArray ConsoleI2cInterface::readNow (unsigned char adr,unsigned int size)
{
    addRead (adr,size);
    addStop();
    preform();
    return getBuffer();
}

QByteArray ConsoleI2cInterface::writeReadNow (unsigned char adr,QByteArray data ,unsigned int size)
{
    addWrite (adr,data);
    addRead (adr,size);
    addStop();
    preform();
    return getBuffer();

}

bool ConsoleI2cInterface::isOk()
{
    return true;
}
