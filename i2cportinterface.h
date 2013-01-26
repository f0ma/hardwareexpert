#ifndef I2CPORTINTERFACE_H
#define I2CPORTINTERFACE_H

#include <QByteArray>
#include "hardwareinterface.h"

class I2cPortInterface : public AbstractHardwareInterface
{
public:
    I2cPortInterface(){}
    virtual ~I2cPortInterface(){}

    virtual bool open()=0;
    virtual void close()=0;

    virtual void checkAsk(bool a)=0;
    virtual void writeNow (unsigned char adr,QByteArray data)=0;
    virtual QByteArray readNow (unsigned char adr,unsigned int size)=0;
    virtual QByteArray writeReadNow (unsigned char adr,QByteArray data ,unsigned int size)=0;
    virtual bool isOk()=0;

    virtual void addWrite (unsigned char adr,QByteArray data)=0;
    virtual void addRead (unsigned char adr,unsigned int size)=0;
    virtual void addStop()=0;
    virtual void addWait(int time)=0;
    virtual bool preform()=0;

    virtual int getBufferLength()=0;
    virtual QByteArray getBuffer()=0;

    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    INTERFACE_TYPE type() {return INTERFACE_TYPE_I2C;}
    virtual bool load() =0;
    virtual void unload() =0;

    virtual void setPortName(QString n)=0;
    virtual QString getPortName()=0;
    virtual QStringList getPortList()=0;

};

#endif // I2CPORTINTERFACE_H
