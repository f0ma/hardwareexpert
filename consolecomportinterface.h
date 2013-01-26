#ifndef CONSOLECOMPORTINTERFACE_H
#define CONSOLECOMPORTINTERFACE_H

#ifndef COMPORTINTERFACE_H
#include "comportinterface.h"
#endif

#include "qextserialport.h"
#include "consoleinterface.h"

#include <QStringList>

class ConsoleComPortInterface : public ComPortInterface
{
    ConsoleInterface * con;

public:
    ConsoleComPortInterface();
    ~ConsoleComPortInterface();

    void setConsole(ConsoleInterface * acon);
    bool open (int baudrate,int flowContol, int partity,int dataBits, int stopBits);
    void close ();
    void write (QByteArray data);
    QByteArray read ();
    int bytesAvailable ();
    void setDTR (bool e);
    void setRTS (bool e);
    void setTimeout (int v);

    bool isAvable() {return true;}
    QString name() {return "CONCOM";}
    QString description() {return QObject::tr("Virtual COM -> Console Interface");}
    QString reason() {return "";}

    bool load() {return true;}
    void unload() {}

    void setPortName(QString n);
    QString getPortName();
    QStringList getPortList();

};

#endif // CONSOLECOMPORTINTERFACE_H
