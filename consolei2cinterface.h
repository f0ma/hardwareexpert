#ifndef CONSOLEI2CINTERFACE_H
#define CONSOLEI2CINTERFACE_H

#include <QList>
#include <QStringList>
#include <i2cportinterface.h>
#include "consoleinterface.h"

class ConsoleI2cInterface : public I2cPortInterface
{
    ConsoleInterface * con;
    QByteArray inputBuffer;

    struct operation
    {
        unsigned char adr;
        QByteArray data;
        unsigned int size;
        unsigned int time;
        enum {READ,WRITE,STOP,WAIT} type;
    };

    QList<operation> operations;

public:
    ConsoleI2cInterface();
    void setConsole(ConsoleInterface * acon);

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

    bool isAvable() {return true;}
    QString name() {return "CONI2C";}
    QString description() {return QObject::tr("Virtual I2C -> Console");}
    QString reason() {return "";}

    bool load()  {return true;}
    void unload() {}

    virtual void setPortName(QString n) {Q_UNUSED(n);}
    virtual QString getPortName() {return "";}
    virtual QStringList getPortList() {return QStringList ();}

};

#endif // CONSOLEI2CINTERFACE_H
