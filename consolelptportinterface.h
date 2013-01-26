#ifndef CONSOLELPTPORTINTERFACE_H
#define CONSOLELPTPORTINTERFACE_H

#include "lptportinterface.h"
#include "consoleinterface.h"
#include <QStringList>

class ConsoleLptPortInterface : public LptPortInterface
{
    ConsoleInterface * con;

public:
    ConsoleLptPortInterface();

    bool open ();

    void setDataPins(unsigned char data);
    unsigned char getDataPins();
    void setCtrlPins(unsigned char data);
    unsigned char getStatPins();

    void setDataModeIn(bool in);

    void close ();
    void setConsole(ConsoleInterface * acon);
    void setPort(QString port) { Q_UNUSED(port);}

    virtual bool isAvable() {return true;}
    virtual QString name() {return "CONLPT";}
    virtual QString description()  {return QObject::tr("Virtual LPT -> Console");}
    virtual QString reason() {return "";}
    INTERFACE_TYPE type() {return AbstractHardwareInterface::INTERFACE_TYPE_LPT;}
    virtual bool load() {return true;}
    virtual void unload() {}

    virtual void setPortName(QString n) {Q_UNUSED(n);}
    virtual QString getPortName() {return "";}
    virtual QStringList getPortList() {return QStringList ();}

};

#endif // CONSOLELPTPORTINTERFACE_H
