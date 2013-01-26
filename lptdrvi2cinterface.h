#ifndef LPTDRVI2CINTERFACE_H
#define LPTDRVI2CINTERFACE_H

#include <QWaitCondition>
#include <QMutex>
#include <QByteArray>
#include <QStringList>

#include "i2cportinterface.h"
#include "lptdrvprovider.h"

class LptDrvI2cInterface : public I2cPortInterface
{
    QByteArray inputBuffer;


    LptDrvProvider prov;


    QString reasonText;

    struct operation
    {
        unsigned char adr;
        QByteArray data;
        unsigned int size;
        unsigned int time;
        enum {READ,WRITE,STOP,WAIT} type;
    };

    QList<operation> operations;

    UINT err;

public:
    LptDrvI2cInterface();

    bool open();
    void close();

    void checkAsk(bool a);
    void writeNow (unsigned char adr,QByteArray data);
    QByteArray readNow (unsigned char adr,unsigned int size);
    QByteArray writeReadNow (unsigned char adr,QByteArray data ,unsigned int size);
    bool isOk();

    void addWrite (unsigned char adr,QByteArray data);
    void addRead (unsigned char adr,unsigned int size);
    void addStop();
    void addWait(int time);
    bool preform();
    int getBufferLength();
    QByteArray getBuffer();

    virtual bool isAvable()
    {
#ifdef Q_OS_LINUX
        return false;
#else
        return true;
#endif
    }

    virtual QString name() {return "MICI2C";}
    virtual QString description()  {return QObject::tr("I2C over Micronas LptDrv - LPT Port");}
    virtual QString reason()
{
#ifdef Q_OS_LINUX
        return QObject::tr("Only for Win32");
#else
        return reasonText;
#endif
}

    bool load();
    void unload();

    void setPortName(QString n);
    QString getPortName();
    QStringList getPortList();
};

#endif // LPTDRVI2CINTERFACE_H
