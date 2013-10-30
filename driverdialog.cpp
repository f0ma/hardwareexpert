///////////////////////////////////////////////////////////////////////
//HardwareExpert. Driver selection dialog.
//Copyright (C) 2011  Stanislav (f0ma) Ivanov
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////

#include "driverdialog.h"
#include "ui_driverdialog.h"

#include  "mainwindow.h"

#include "settings.h"

DriverDialog::DriverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DriverDialog)
{
    ui->setupUi(this);

    initComplete=false;

    ConsoleComPortInterface * cCom = new ConsoleComPortInterface();
    ConsoleLptPortInterface * cLpt = new ConsoleLptPortInterface();
    ConsoleI2cInterface * cI2c = new ConsoleI2cInterface();
    ConsoleDirectIOInterface * cDio = new ConsoleDirectIOInterface();

    MainWindow * mw = dynamic_cast<MainWindow *>(this->parent());

    cCom->setConsole(mw->getConsole());
    cLpt->setConsole(mw->getConsole());
    cI2c->setConsole(mw->getConsole());
    cDio->setConsole(mw->getConsole());

    WinIOLptPortInterface * wioc = new WinIOLptPortInterface();

    LptWinIODirectIOInterface * wiod = new LptWinIODirectIOInterface();
    wiod->setInterface(wioc);

    FtdiInterfaceProvider * pftdi = new FtdiInterfaceProvider();

    interfaces << cCom
               << new ExcComInterface()
               << cLpt
               << wioc
               << cI2c
               << new LptDrvI2cInterface()
               << cDio
               << wiod
               << new FtdiDioPortInterface(pftdi);

    AbstractHardwareInterface * i;

    foreach(i,interfaces)
    {

    if(i->isAvable())
        {
        if(i->load())
        {
            ui->deviceInfo->append(i->name() + " ("+i->description() +tr("):[OK]"));
            if(i->type()==AbstractHardwareInterface::INTERFACE_TYPE_COM) comInterfaces << i;
            if(i->type()==AbstractHardwareInterface::INTERFACE_TYPE_LPT) lptInterfaces << i;
            if(i->type()==AbstractHardwareInterface::INTERFACE_TYPE_I2C) i2cInterfaces << i;
            if(i->type()==AbstractHardwareInterface::INTERFACE_TYPE_DIRECT_IO) dioInterfaces << i;
        }
        else ui->deviceInfo->append(i->name() + " ("+i->description() +tr("):[Error] - ")+i->reason());
        }
    else ui->deviceInfo->append(i->name() + " ("+i->description() +tr("):[Unavable] - ")+i->reason());
    }


    foreach(i,comInterfaces)
    {
        ui->cbComIfSelect->addItem(i->description());
    }

    foreach(i,lptInterfaces)
    {
        ui->cbLptIfSelect->addItem(i->description());
    }

    foreach(i,i2cInterfaces)
    {
        ui->cbI2cIfSelect->addItem(i->description());
    }

    foreach(i,dioInterfaces)
    {
        ui->cbDioIfSelect->addItem(i->description());
    }

    use_settings;

    ui->cbComIfSelect->setCurrentIndex(settings.value("drivers/currentComDriver",0).toInt());
    ui->cbLptIfSelect->setCurrentIndex(settings.value("drivers/currentLptDriver",0).toInt());
    ui->cbI2cIfSelect->setCurrentIndex(settings.value("drivers/currentI2cDriver",0).toInt());
    ui->cbDioIfSelect->setCurrentIndex(settings.value("drivers/currentDioDriver",0).toInt());

    initComplete = true;
}

ComPortInterface * DriverDialog::getCurrentComPort()
{
    if (comInterfaces.length()==0) return NULL;
    return dynamic_cast <ComPortInterface *> (comInterfaces[ui->cbComIfSelect->currentIndex()]);
}

LptPortInterface * DriverDialog::getCurrentLptPort()
{
    if (lptInterfaces.length()==0) return NULL;
    return dynamic_cast <LptPortInterface *> (lptInterfaces[ui->cbLptIfSelect->currentIndex()]);
}

I2cPortInterface * DriverDialog::getCurrentI2cPort()
{
    if (i2cInterfaces.length()==0) return NULL;
    if(ui->cbI2cIfSelect->currentIndex()<0) return dynamic_cast <I2cPortInterface *> (i2cInterfaces[0]);
    return dynamic_cast <I2cPortInterface *> (i2cInterfaces[ui->cbI2cIfSelect->currentIndex()]);
}

DirectIOInterface * DriverDialog::getCurrentDioPort()
{
    if (dioInterfaces.length()==0) return NULL;
    return dynamic_cast <DirectIOInterface *> (dioInterfaces[ui->cbDioIfSelect->currentIndex()]);
}


DriverDialog::~DriverDialog()
{
    delete ui;
}

void DriverDialog::on_cbComIfSelect_currentIndexChanged(int index)
{
    updateList(ui->cbComPortName,getCurrentComPort()->getPortList());

    use_settings;
    ui->cbComPortName->setCurrentIndex(settings.value("drivers/"+getCurrentComPort()->name(),0).toInt());

    if(!initComplete)return;

    settings.setValue("drivers/currentComDriver",index);
}

void DriverDialog::on_cbLptIfSelect_currentIndexChanged(int index)
{
    updateList(ui->cbLptName,getCurrentLptPort()->getPortList());
    use_settings;

    ui->cbLptName->setCurrentIndex(settings.value("drivers/"+getCurrentLptPort()->name(),0).toInt());

    if(!initComplete)return;

    settings.setValue("drivers/currentLptDriver",index);
}

void DriverDialog::on_cbI2cIfSelect_currentIndexChanged(int index)
{
    if(!initComplete)return;

    updateList(ui->cbI2cPortName,getCurrentI2cPort()->getPortList());

    use_settings;

    ui->cbI2cPortName->setCurrentIndex(settings.value("drivers/"+getCurrentI2cPort()->name(),0).toInt());

    settings.setValue("drivers/currentI2cDriver",index);
}

void DriverDialog::updateList(QComboBox * list,QStringList values)
{
        bool initC = initComplete;

        initComplete = false;

        list->clear();

        if(values.indexOf("")!=-1) list->setEditable(false);
        else list->setEditable(true);

        if(values.isEmpty())list->setEnabled(false);
        else list->setEnabled(true);

        values.removeOne("");
        list->addItems(values);

        initComplete = initC;
}

void DriverDialog::on_cbComPortName_currentIndexChanged(QString n)
{
    getCurrentComPort()->setPortName(n);

    if(!initComplete)return;

    use_settings;
    settings.setValue("drivers/"+getCurrentComPort()->name(),ui->cbComPortName->currentIndex());
}

void DriverDialog::on_cbLptName_currentIndexChanged(QString n)
{
    getCurrentLptPort()->setPortName(n);

    if(!initComplete)return;

    use_settings;
    settings.setValue("drivers/"+getCurrentLptPort()->name(),ui->cbLptName->currentIndex());
}

void DriverDialog::on_cbI2cPortName_currentIndexChanged(QString n)
{
    getCurrentI2cPort()->setPortName(n);

    if(!initComplete)return;

    use_settings;
    settings.setValue("drivers/"+getCurrentI2cPort()->name(),ui->cbI2cPortName->currentIndex());
}

void DriverDialog::on_cbDioIfSelect_currentIndexChanged(int index)
{
    updateList(ui->cbDioPortName,getCurrentDioPort()->getPortList());

    use_settings;

    ui->cbI2cPortName->setCurrentIndex(settings.value("drivers/"+getCurrentDioPort()->name(),0).toInt());

    if(!initComplete)return;

    settings.setValue("drivers/currentDioDriver",index);
}

void DriverDialog::on_cbDioPortName_currentIndexChanged(const QString &arg1)
{
    getCurrentDioPort()->setPortName(arg1);

    if(!initComplete)return;

    use_settings;
    settings.setValue("drivers/"+getCurrentDioPort()->name(),ui->cbDioPortName->currentIndex());
}
