#ifndef LPTWINIODIRECTIOINTERFACE_H
#define LPTWINIODIRECTIOINTERFACE_H

#include "directiointerface.h"
#include "winiolptportinterface.h"


class LptWinIODirectIOInterface : public DirectIOInterface
{

    WinIOLptPortInterface * wiop;

public:
    LptWinIODirectIOInterface();
    ~LptWinIODirectIOInterface() {}

    void setInterface(WinIOLptPortInterface * w);

    bool isAvable()
    {
#ifdef Q_OS_LINUX
        return false;
#else
        return true;
#endif
    }

    QString name() {return "DIOtoLPT";}
    QString description() {return QObject::tr("DIRECTIO -> WIOLPT -> LPT");}
    QString reason(){return wiop->reason();}

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

    bool load();
    void unload();


};

#endif // LPTWINIODIRECTIOINTERFACE_H
