#ifndef CONSOLEDIRECTIOINTERFACE_H
#define CONSOLEDIRECTIOINTERFACE_H

#include "directiointerface.h"
#include "consoleinterface.h"

class ConsoleDirectIOInterface : public DirectIOInterface
{
    ConsoleInterface * con;
    bool opened;

public:
    ConsoleDirectIOInterface();
    ~ConsoleDirectIOInterface();

    void setConsole(ConsoleInterface * acon) {con = acon;}

    bool isAvable() {return true;}
    QString name() {return "CONDIO";}
    QString description() {return QObject::tr("DIRECTIO -> Console");}
    QString reason() {return "";}

    int getPortCount();
    void setPortValue(int no,unsigned char value);
    unsigned char getPortValue(int no);

    void setPinMode(int no,int pin,IO_MODE mode) ;
    void setPortMode(int no,IO_MODE mode) ;

    void setPortName(QString n);
    QString getPortName();
    QStringList getPortList();

    bool open();
    void close();

    bool load() { return true; }
    void unload() {}


};

#endif // CONSOLEDIRECTIOINTERFACE_H
