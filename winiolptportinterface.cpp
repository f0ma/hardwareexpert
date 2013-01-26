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

#include "winiolptportinterface.h"
#include "settings.h"

bool WinIOLptPortInterface::load()
{
int l = prov.load();

if (l==1) return true;

if (l==0) reasonText = QObject::tr("Can't find WinIO library!");

if (l==-255) reasonText = QObject::tr("Can't load WinIO driver!");

return false;
}

void WinIOLptPortInterface::unload()
{
prov.unload();
}

WinIOLptPortInterface::WinIOLptPortInterface()
{
    ctrlstate=0x00;    //0x20

}

bool WinIOLptPortInterface::open ()
{
//    use_settings;

//    unsigned char v = prov.getPort(basePort+0x402); //select EPP mode in ECP
//    prov.setPort(basePort+0x402,v);

//    settings.setValue("drivers/lptModeByte",v);

    prov.setPort(basePort+0x402,0x35); //select Bi-Dir mode in ECP

    return true; ///WARNING:hardcoded


}

void WinIOLptPortInterface::setPortName (QString n)
{
currPortName = n;
if(n=="LPT1")basePort=0x378;
if(n=="LPT2")basePort=0x278;
if(n=="LPT3")basePort=0x3BC;
}

QString WinIOLptPortInterface::getPortName()
{
    return currPortName;
}

QStringList WinIOLptPortInterface::getPortList()
{
    QStringList l;
    l<< "LPT1"<< "LPT2"<< "LPT3";
    return l;

}


void WinIOLptPortInterface::setDataPins(unsigned char data)
{
    prov.setPort(basePort,data);
}

unsigned char WinIOLptPortInterface::getDataPins()
{
    return     prov.getPort(basePort);
}

void WinIOLptPortInterface::setCtrlPins(unsigned char data)
{
    //unsigned char data
    unsigned char invdata = ~data;

    data = data & 0x4;            //00000100
    invdata = invdata & 0xB;      //00001011

    ctrlstate = (ctrlstate & 0xF0) + (data + invdata);

    prov.setPort(basePort+2,ctrlstate);
}

unsigned char WinIOLptPortInterface::getStatPins()
{
//    * D0: state not specified
//    * D1: state not specified
//    * D2: state not specified
//    * D3: state of pin 15 (ERROR)
//    * D4: state of pin 13 (SELECTED)
//    * D5: state of pin 12 (PAPER OUT)
//    * D6: state of pin 10 (ACK)
//    * D7: state of pin 11 (BUSY) inverted
unsigned char state = prov.getPort(basePort+1);
unsigned char inv = ~state;

state=state & 0x78;  //01111000
                     //I    UUU
inv=inv & 0x80;      //10000000
                     //I    UUU

return state + inv;

}

void WinIOLptPortInterface::setDataModeIn(bool in)
{
    if(in)ctrlstate = (ctrlstate & 0x0F) + 0x20;
    else ctrlstate = (ctrlstate & 0x0F) + 0x00;
    prov.setPort(basePort+2,ctrlstate);
}

void WinIOLptPortInterface::close ()
{

}


