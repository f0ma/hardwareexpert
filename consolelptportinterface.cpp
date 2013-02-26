///////////////////////////////////////////////////////////////////////
//HardwareExpert. Print only interface for LPT port.
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

#include "consolelptportinterface.h"

ConsoleLptPortInterface::ConsoleLptPortInterface()
{
    con=NULL;
    opened = false;
}

bool ConsoleLptPortInterface::open ()
{
    con->print(QObject::tr("Console LPT port opened\n"));
    opened = true;
    return true;
}

void ConsoleLptPortInterface::setDataPins(unsigned char data)
{
    QString s;
    s.setNum(data,16);

    con->print(QObject::tr("Set data pins: ")+s.toUpper()+"\n");
}

unsigned char ConsoleLptPortInterface::getDataPins()
{
    con->print(QObject::tr("Getting data pins\n"));
    return 0;
}

void ConsoleLptPortInterface::setCtrlPins(unsigned char data)
{
    QString s;
    s.setNum(data,16);

    con->print(QObject::tr("Set control pins: ")+s.toUpper()+"\n");

}

unsigned char ConsoleLptPortInterface::getStatPins()
{
    con->print(QObject::tr("Getting status pins\n"));
    return 0;
}

void ConsoleLptPortInterface::setDataModeIn(bool in)
{
    if(in)con->print(QObject::tr("LPT port mode: INPUT\n"));
    else con->print(QObject::tr("LPT port mode: OUTPUT\n"));
}

void ConsoleLptPortInterface::close ()
{
    if(opened)con->print(QObject::tr("Console LPT port closed\n"));
    opened = false;
}

void ConsoleLptPortInterface::setConsole(ConsoleInterface * acon)
{
    con=acon;
}

