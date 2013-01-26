///////////////////////////////////////////////////////////////////////
//HardwareExpert. Option window.
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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include "settings.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    use_settings;
    ui->cbUseBackup->setChecked(settings.value("useBackup",0).toInt()==2);
    ui->cbExecuteNow->setChecked(settings.value("executeNow",0).toInt()==2);
    ui->cbExecuteNow->setChecked(settings.value("executeNow",0).toInt()==2);
    ui->cbTrunkLog->setChecked(settings.value("trunkLog",0).toInt()==2);
    ui->lePath->setText(settings.value("helpPath","").toString());

    on_cbTrunkLog_stateChanged(settings.value("trunkLog",0).toInt());

    ui->sbTrunkLen->setValue(settings.value("trunkLen",100).toInt());

    QStringList items = settings.value("scriptPaths",QStringList()).toStringList();
    for(int i=0;i<items.count();i++) ui->lwScriptPaths->addItem(items[i]);

    ui->tabWidget->setCurrentIndex(0);



}

void OptionsDialog::sendPathList()
{
    QStringList items;
    for(int i=0;i<ui->lwScriptPaths->count();i++) items << ui->lwScriptPaths->item(i)->data(0).toString();

    items << QCoreApplication::applicationDirPath() + "/scripts";

    emit updateScriptLibrary(items);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::on_cbUseBackup_stateChanged(int state)
{
    use_settings;
    settings.setValue("useBackup",state);
}

void OptionsDialog::on_cbExecuteNow_stateChanged(int state)
{
    use_settings;
    settings.setValue("executeNow",state);
}

void OptionsDialog::on_bpBrowse_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this,tr("Path to help files"),ui->lePath->text());
    if(s!="")
    {
        ui->lePath->setText(s);
        use_settings;
        settings.setValue("helpPath",s);
    }
}


void OptionsDialog::on_deleteButton_clicked()
{
    delete ui->lwScriptPaths->takeItem(ui->lwScriptPaths->currentRow());

    QStringList items;
    for(int i=0;i<ui->lwScriptPaths->count();i++) items << ui->lwScriptPaths->item(i)->data(0).toString();

    use_settings;
    settings.setValue("scriptPaths",items);

    emit updateScriptLibrary(items);

}

void OptionsDialog::on_addButton_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this,tr("Directory with scripts"));
    if(s!="")
    {
        ui->lwScriptPaths->addItem(s);

        QStringList items;
        for(int i=0;i<ui->lwScriptPaths->count();i++) items << ui->lwScriptPaths->item(i)->data(0).toString();

        use_settings;
        settings.setValue("scriptPaths",items);

        emit updateScriptLibrary(items);

    }
}




void OptionsDialog::on_cbTrunkLog_stateChanged(int state)
{
    use_settings;
    settings.setValue("trunkLog",state);
    if (state!=2) ui->sbTrunkLen->setEnabled(false);
    else ui->sbTrunkLen->setEnabled(true);
}

void OptionsDialog::on_sbTrunkLen_valueChanged(int val)
{
    use_settings;
    settings.setValue("trunkLen",val);
}

void OptionsDialog::on_cbSaveGuiState_stateChanged(int val)
{
    use_settings;
    settings.setValue("saveGui",val);
}
