#ifndef DIRECTIOINTERFACE_H
#define DIRECTIOINTERFACE_H

#include "hardwareinterface.h"

class DirectIOInterface : public AbstractHardwareInterface
{
public:
    DirectIOInterface() {}

    enum IO_MODE {IO_MODE_INPUT,IO_MODE_OUTPUT,IO_MODE_TRISTATE};


    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    virtual INTERFACE_TYPE type() {return AbstractHardwareInterface::INTERFACE_TYPE_DIRECT_IO;}

    virtual int getPortCount()=0;
    virtual void setPortValue(int no,unsigned char value)=0;
    virtual unsigned char getPortValue(int no)=0;

    virtual void setPinMode(int no,int pin,IO_MODE mode)=0;
    virtual void setPortMode(int no,IO_MODE mode)=0;

    virtual bool open() =0;
    virtual void close() =0;

    virtual bool load() =0;
    virtual void unload() =0;

    virtual void setPortName(QString n)=0;
    virtual QString getPortName()=0;
    virtual QStringList getPortList()=0;

    virtual ~DirectIOInterface(){}
};

#endif // DIRECTIOINTERFACE_H
