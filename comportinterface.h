#ifndef COMPORTINTERFACE_H
#define COMPORTINTERFACE_H

#include <QByteArray>
#include "hardwareinterface.h"

class ComPortInterface : public AbstractHardwareInterface
{
public:
    ComPortInterface(){}
    virtual bool open (int baudrate,int flowContol, int partity,int dataBits, int stopBits)=0;
    virtual void close ()=0;
    virtual void write (QByteArray data)=0;
    virtual QByteArray read ()=0;
    virtual int bytesAvailable ()=0;
    virtual void setDTR (bool e)=0;
    virtual void setRTS (bool e)=0;
    virtual void setTimeout (int v)=0;

    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    virtual INTERFACE_TYPE type(){return AbstractHardwareInterface::INTERFACE_TYPE_COM;}
    virtual bool load() =0;
    virtual void unload() =0;

    virtual void setPortName(QString n)=0;
    virtual QString getPortName()=0;
    virtual QStringList getPortList()=0;

    virtual ~ComPortInterface(){}

};

#endif // COMPORTINTERFACE_H
