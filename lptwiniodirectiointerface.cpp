#include "lptwiniodirectiointerface.h"

LptWinIODirectIOInterface::LptWinIODirectIOInterface()
{
}


void LptWinIODirectIOInterface::setInterface(WinIOLptPortInterface * w)
{
    wiop = w;
}

int LptWinIODirectIOInterface::getPortCount()
{
    return 3;
}

void LptWinIODirectIOInterface::setPortValue(int no,unsigned char value)
{
    if (no == 0) wiop->setDataPins(value);
//    if (no == 1)
    if (no == 2) wiop->setCtrlPins(value);
}

unsigned char LptWinIODirectIOInterface::getPortValue(int no)
{
    if (no == 0) return wiop->getDataPins();
    if (no == 1) return wiop->getStatPins();
    return 0;
}

void LptWinIODirectIOInterface::setPinMode(int no,int pin,IO_MODE mode)
{
    Q_UNUSED(no);
    Q_UNUSED(pin);
    Q_UNUSED(mode);
}

void LptWinIODirectIOInterface::setPortMode(int no,IO_MODE mode)
{
    if (no == 0) wiop->setDataModeIn(mode == IO_MODE_INPUT);
}

void LptWinIODirectIOInterface::setPortName(QString n)
{
    wiop->setPortName(n);
}

QString LptWinIODirectIOInterface::getPortName()
{
    return wiop->getPortName();
}

QStringList LptWinIODirectIOInterface::getPortList()
{
    return  wiop->getPortList();
}

bool LptWinIODirectIOInterface::open()
{
    return wiop->open();
}

void LptWinIODirectIOInterface::close()
{
    wiop->close();
}

bool LptWinIODirectIOInterface::load()
{
    return wiop->load();
}

void LptWinIODirectIOInterface::unload()
{
    return wiop->unload();
}
