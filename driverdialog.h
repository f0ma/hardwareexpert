#ifndef DRIVERDIALOG_H
#define DRIVERDIALOG_H

#include <QDialog>
#include <QComboBox>

#include "hardwareinterface.h"

#include "consolecomportinterface.h"
#include "exccominterface.h"

#include "consolelptportinterface.h"
#include "lptportinterface.h"
#include "winiolptportinterface.h"


#include "i2cportinterface.h"
#include "lptdrvi2cinterface.h"
#include "consolei2cinterface.h"

#include "lptdrvprovider.h"
#include "winioprovider.h"

namespace Ui {
    class DriverDialog;
}

class DriverDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DriverDialog(QWidget *parent = 0);
    ~DriverDialog();
    bool initComplete;

    ComPortInterface * getCurrentComPort();
    LptPortInterface * getCurrentLptPort();
    I2cPortInterface * getCurrentI2cPort();

private:
    Ui::DriverDialog *ui;

    QList <AbstractHardwareInterface *> interfaces;
    QList <AbstractHardwareInterface *> comInterfaces;
    QList <AbstractHardwareInterface *> lptInterfaces;
    QList <AbstractHardwareInterface *> i2cInterfaces;

    void updateList(QComboBox * list,QStringList values);

private slots:
    void on_cbI2cPortName_currentIndexChanged(QString );
    void on_cbLptName_currentIndexChanged(QString );
    void on_cbComPortName_currentIndexChanged(QString );
    void on_cbI2cIfSelect_currentIndexChanged(int index);
    void on_cbLptIfSelect_currentIndexChanged(int index);
    void on_cbComIfSelect_currentIndexChanged(int index);
};

#endif // DRIVERDIALOG_H


