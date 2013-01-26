#ifndef LPTPORTINTERFACE_H
#define LPTPORTINTERFACE_H

#include "hardwareinterface.h"

class LptPortInterface : public AbstractHardwareInterface
{
public:
    LptPortInterface(){}
    virtual bool open ()=0;

    virtual void setDataPins(unsigned char data)=0;
    virtual unsigned char getDataPins()=0;
    virtual void setCtrlPins(unsigned char data)=0;
    virtual unsigned char getStatPins()=0;

    virtual void setDataModeIn(bool in)=0;

    virtual void close ()=0;



    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    INTERFACE_TYPE type() {return AbstractHardwareInterface::INTERFACE_TYPE_LPT;}
    virtual bool load() =0;
    virtual void unload() =0;

    virtual void setPortName(QString n)=0;
    virtual QString getPortName()=0;
    virtual QStringList getPortList()=0;

    virtual ~LptPortInterface(){}

};

#endif // LPTPORTINTERFACE_H
