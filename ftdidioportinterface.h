#ifndef FTDIDIOPORTINTERFACE_H
#define FTDIDIOPORTINTERFACE_H

#include <QString>
#include <QStringList>
#include "directiointerface.h"
#include "ftdiinterfaceprovider.h"

class FtdiDioPortInterface : public DirectIOInterface
{
    QString reasonText;
    QList<QString> portSerials;
    QList<FT_HANDLE> ports;
    QList<unsigned char> bitmodes;
    FtdiInterfaceProvider * prov;

public:
    FtdiDioPortInterface(FtdiInterfaceProvider * provider);

    bool isAvable();

    int getPortCount();
    void setPortValue(int no,unsigned char value);
    unsigned char getPortValue(int no);

    void setPinMode(int no,int pin,IO_MODE mode);
    void setPortMode(int no,IO_MODE mode);

    bool open();
    void close();

    bool load();
    void unload();

    void setPortName(QString n);
    QString getPortName();
    QStringList getPortList();


    virtual QString name() {return "DIOFTDI";}
    virtual QString description()  {return QObject::tr("DirectIO to FTDI");}
    virtual QString reason() {return reasonText;}

};

#endif // FTDIDIOPORTINTERFACE_H
