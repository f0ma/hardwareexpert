#ifndef FTDII2CINTERFACE_H
#define FTDII2CINTERFACE_H

#include <QWaitCondition>
#include <QMutex>
#include <QByteArray>
#include <QStringList>

#include "i2cportinterface.h"
#include "ftdiinterfaceprovider.h"

class FtdiI2cInterface : public I2cPortInterface
{
    QByteArray inputBuffer;

   FtdiInterfaceProvider* prov;

    QString reasonText;



public:
    FtdiI2cInterface(FtdiInterfaceProvider* prov);

    bool open();
    void close();

    void checkAsk(bool a);
    void writeNow (unsigned char adr,QByteArray data);
    QByteArray readNow (unsigned char adr,unsigned int size);
    QByteArray writeReadNow (unsigned char adr,QByteArray data ,unsigned int size);
    bool isOk();

    void addWrite (unsigned char adr,QByteArray data) {};
    void addRead (unsigned char adr,unsigned int size) {};
    void addStop() {};
    void addWait(int time) {};
    bool preform() {};
    int getBufferLength() {};
    QByteArray getBuffer() {};

    virtual bool isAvable()
    {
        return true;
    }

    virtual QString name() {return "FTDII2C";}
    virtual QString description()  {return QObject::tr("I2C over FTDI chip");}
    virtual QString reason()
{
        return reasonText;
}

    bool load();
    void unload();

    void setPortName(QString n);
    QString getPortName();
    QStringList getPortList();
};

#endif // FTDII2CINTERFACE_H
