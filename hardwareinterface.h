#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H

#include <QString>

class AbstractHardwareInterface
{
public:

    enum INTERFACE_TYPE {INTERFACE_TYPE_COM,
                         INTERFACE_TYPE_LPT,
                         INTERFACE_TYPE_I2C,
                         INTERFACE_TYPE_DIRECT_IO,
                         INTERFACE_TYPE_USB};


    AbstractHardwareInterface(){}
    virtual bool isAvable() =0;
    virtual QString name() =0;
    virtual QString description() =0;
    virtual QString reason() =0;
    virtual INTERFACE_TYPE type() =0;
    virtual bool load() =0;
    virtual void unload() =0;
    virtual ~AbstractHardwareInterface(){}
};

#endif // HARDWAREINTERFACE_H
