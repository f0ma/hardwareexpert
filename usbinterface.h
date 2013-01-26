#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include "hardwareinterface.h"

class UsbPortInterface : public AbstractHardwareInterface
{
public:
    UsbPortInterface();

    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    virtual INTERFACE_TYPE type() {return AbstractHardwareInterface::INTERFACE_TYPE_USB;}

    virtual bool open (int vid,int pid) =0;
    virtual void close ()=0;
    virtual void reset ()=0;
    virtual bool bulk_write(QByteArray data,int endpoint,int timeout)=0;
    virtual QByteArray bulk_read(int len,int endpoint,int timeout)=0;
    virtual QByteArray control_msg(unsigned char cmsgtype,unsigned char cmsgrequest, int iface, int len)=0;

    virtual bool load() =0;
    virtual void unload() =0;
    virtual ~UsbPortInterface(){};
};

#endif // USBINTERFACE_H
