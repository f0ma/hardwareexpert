#include "consoledirectiointerface.h"

ConsoleDirectIOInterface::ConsoleDirectIOInterface()
{
    opened = false;
}

ConsoleDirectIOInterface::~ConsoleDirectIOInterface()
{
}


void ConsoleDirectIOInterface::setPinMode(int no,int pin,IO_MODE mode)
{
    QString r;

    if(mode==IO_MODE_INPUT) r = "INPUT";
    if(mode==IO_MODE_OUTPUT) r = "OUTPUT";
    if(mode==IO_MODE_TRISTATE) r = "TRISTATE";


    con->print(QString("Setting pin %1 in port %2 to mode %3\n").arg(pin).arg(no).arg(r));
}

void ConsoleDirectIOInterface::setPortMode(int no,IO_MODE mode)
{
    QString r;

    if(mode==IO_MODE_INPUT) r = "INPUT";
    if(mode==IO_MODE_OUTPUT) r = "OUTPUT";
    if(mode==IO_MODE_TRISTATE) r = "TRISTATE";

    con->print(QString("Setting port %1 to mode %2\n").arg(no).arg(r));
}


void ConsoleDirectIOInterface::setPortName(QString n)
{
    Q_UNUSED(n);
}

QString ConsoleDirectIOInterface::getPortName()
{
    return "";
}

QStringList ConsoleDirectIOInterface::getPortList()
{
    return QStringList();
}

int ConsoleDirectIOInterface::getPortCount()
{
    return 16;
}

void ConsoleDirectIOInterface::setPortValue(int no,unsigned char value)
{
    con->print(QString("Setting port %1 to value %2\n").arg(no).arg(value));
}

unsigned char ConsoleDirectIOInterface::getPortValue(int no)
{
    con->print(QString("Getting value from port %1\n").arg(no));
    return 0;
}

bool ConsoleDirectIOInterface::open()
{
    opened = true;
    con->print(QString("Virtual DIO port opened\n"));
    return true;
}

void ConsoleDirectIOInterface::close()
{
    if(opened)
    {
        con->print(QString("Virtual DIO port closed\n"));
        opened = false;
    }
}
