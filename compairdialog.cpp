///////////////////////////////////////////////////////////////////////
//HardwareExpert. Compare file dialog.
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

#include "compairdialog.h"
#include "ui_compairdialog.h"

CompairDialog::CompairDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompairDialog)
{
    ui->setupUi(this);
    connect(ui->cbA,SIGNAL(currentIndexChanged(int)),this,SLOT(updateComboA(int)));
    connect(ui->cbB,SIGNAL(currentIndexChanged(int)),this,SLOT(updateComboB(int)));

    ui->lvResults->setModel(&resultModel);



}

CompairDialog::~CompairDialog()
{
    delete ui;
}

void CompairDialog::closeEvent(QCloseEvent *event)
 {
    QByteArray empty;
    if(aEditor!=NULL)aEditor->setExternalHighlight(empty);
    if(bEditor!=NULL)bEditor->setExternalHighlight(empty);

    if(aEditor!=NULL && bEditor!=NULL)
    {
    disconnect(aEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),bEditor->verticalScrollBar(),SLOT(setValue(int)));
    disconnect(bEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),aEditor->verticalScrollBar(),SLOT(setValue(int)));
}

    event->accept();
 }

void CompairDialog::addEditor(HexEditorWidget * ed, QString name)
{
    edList.append(ed);
    edNames.append(name);
    updateCombo();
}

void CompairDialog::updateCombo()
{
cLock = true;

ui->cbA->clear();
ui->cbB->clear();
ui->cbA->addItem(tr("Not selected"));
ui->cbB->addItem(tr("Not selected"));

QString s;
foreach(s,edNames)
{
ui->cbB->addItem(s);
ui->cbA->addItem(s);
}
        aEditor = NULL;
        bEditor = NULL;


if(edNames.count()>0)
{

if(edNames.count() == 2)
{
ui->cbA->setCurrentIndex(1);
ui->cbB->setCurrentIndex(2);
}

if(edNames.count() > 2)
{
ui->cbA->setCurrentIndex(1);
ui->cbB->setCurrentIndex(0);
}

}

cLock = false;

updateComboA(ui->cbA->currentIndex());
updateComboB(ui->cbB->currentIndex());

}

void CompairDialog::updateComboA(int n)
{
if(cLock)return;

    if(ui->cbA->currentIndex() == ui->cbB->currentIndex())
    {
    if (ui->cbA->currentIndex()!=0)
        {
        ui->cbB->setCurrentIndex(0);
        bEditor = NULL;
    }
return;
}
    if (n!=0)
        aEditor = edList.at(n-1);
    else
        aEditor = NULL;


    if(aEditor!=NULL && bEditor!=NULL)
    {
    disconnect(aEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),bEditor->verticalScrollBar(),SLOT(setValue(int)));
    disconnect(bEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),aEditor->verticalScrollBar(),SLOT(setValue(int)));
}
}


void CompairDialog::updateComboB(int n)
{
if(cLock)return;

    if(ui->cbA->currentIndex() == ui->cbB->currentIndex())
    {
    if (ui->cbB->currentIndex()!=0)
        {
    ui->cbA->setCurrentIndex(0);
        aEditor = NULL;
}
    return;
    }
        if (n!=0)
            bEditor = edList.at(n-1);
        else
            bEditor = NULL;

        if(aEditor!=NULL && bEditor!=NULL)
        {
        disconnect(aEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),bEditor->verticalScrollBar(),SLOT(setValue(int)));
        disconnect(bEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),aEditor->verticalScrollBar(),SLOT(setValue(int)));
    }
    }


void CompairDialog::on_pbCompare_clicked()
{

    if(aEditor!=NULL && bEditor!=NULL)
    {
    result.clear();
    resultModel.setStringList(result);

    connect(aEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),bEditor->verticalScrollBar(),SLOT(setValue(int)));
    connect(bEditor->verticalScrollBar(),SIGNAL(sliderMoved(int)),aEditor->verticalScrollBar(),SLOT(setValue(int)));


    QByteArray dataA = aEditor->data();
    QByteArray dataB = bEditor->data();
    QByteArray hl;
    if(dataA.size()>dataB.size())  hl.fill('\0',dataA.size());
    else hl.fill('\0',dataB.size());

    int sCount=0;
    int rCount=0;
    int i;

    for(i=0;i<dataA.size() && i<dataB.size();i++)
    {
        if(dataA.at(i)==dataB.at(i))
        {
        if(sCount!=0)
        {
        QString sAdr;
        QString sLen;

        sAdr.setNum(i-sCount,16);
        sLen.setNum(sCount,16);

        result << "0x" + sAdr.toUpper()+ " | " + "0x" + sLen.toUpper();

        for(int j =i-sCount;j<i;j++)
        {
            hl[j]='C';
        }

        sCount = 0;
        }
        }
        else
        {
            sCount++;
            rCount++;
        }
    }

    if(sCount!=0)
    {
    QString sAdr;
    QString sLen;

    sAdr.setNum(i-sCount,16);
    sLen.setNum(sCount,16);

    result << "0x" + sAdr.toUpper()+ " | " + "0x" + sLen.toUpper();

    for(int j =i-sCount;j<i;j++)
    {
        hl[j]='C';
    }

    sCount = 0;
    }

    if(dataA.size() != dataB.size())
    {
    rCount++;
    int min;
    int max;
    QString txt;

    if(dataA.size() > dataB.size())
    {
        txt = tr("Only in A");
        min = dataB.size();
        max = dataA.size();
    }
    else
    {
        txt = tr("Only in B");
        max = dataB.size();
        min = dataA.size();
    }

    for(int j = min;j<max;j++)
    {
        hl[j]='C';
    }

    QString sAdr;
    QString sLen;

    sAdr.setNum(min,16);
    sLen.setNum(max-min,16);

    result <<  "0x" + sAdr.toUpper()+ " | " + "0x" + sLen.toUpper() + " " + txt;


    }



    if(rCount==0)
    {
        result <<  tr("No differences");
        QByteArray empty;
        aEditor->setExternalHighlight(empty);
        bEditor->setExternalHighlight(empty);
    }
    else
    {
        aEditor->setExternalHighlight(hl);
        bEditor->setExternalHighlight(hl);

    }

        resultModel.setStringList(result);
}
}

void CompairDialog::on_lvResults_clicked(const QModelIndex &index)
{
    QStringList asl = index.data().toString().split("|");
    if(asl.length()==0)return;
    aEditor->gotoAddress(asl.at(0).toInt(NULL,16));
    bEditor->gotoAddress(asl.at(0).toInt(NULL,16));
}
