#ifndef WINIOLPTPORTINTERFACE_H
#define WINIOLPTPORTINTERFACE_H

#include <QString>
#include <QStringList>
#include "lptportinterface.h"
#include "winioprovider.h"

class WinIOLptPortInterface : public LptPortInterface
{
    WinIOProvider prov;
    int basePort;
    unsigned char ctrlstate;
    QString reasonText;
    QString currPortName;

public:
    WinIOLptPortInterface();

    bool open ();

    void setDataPins(unsigned char data);
    unsigned char getDataPins();
    void setCtrlPins(unsigned char data);
    unsigned char getStatPins();

    void setDataModeIn(bool in);

    void close ();


    virtual bool isAvable()
    {
#ifdef Q_OS_LINUX
        return false;
#else
        return true;
#endif
    }

    virtual QString name() {return "WIOLPT";}
    virtual QString description()  {return QObject::tr("LPT Port over WinIO");}
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

#endif // WINIOLPTPORTINTERFACE_H
