#ifndef EXCCOMINTERFACE_H
#define EXCCOMINTERFACE_H

#ifndef COMPORTINTERFACE_H
#include "comportinterface.h"
#endif
#include "qextserialport.h"

#include <QStringList>

class ExcComInterface : public ComPortInterface
{
QextSerialPort * port;
QString portName;

public:
    ExcComInterface();
    ~ExcComInterface();

    void setPortName(QString n);
    bool open (int baudrate,int flowContol, int partity,int dataBits, int stopBits);
    void close ();
    void write (QByteArray data);
    QByteArray read ();
    int bytesAvailable ();
    void setDTR (bool e);
    void setRTS (bool e);
    void setTimeout(int v);

    bool isAvable() {return true;}
    QString name() {return "EXTCOM";}
    QString description() {return QObject::tr("COM Port over QExtSerialPort");}
    QString reason() {return "";}

    bool load() {return true;}
    void unload() {}

    QString getPortName();
    QStringList getPortList();


};

#endif // EXCCOMINTERFACE_H

