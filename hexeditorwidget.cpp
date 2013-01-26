///////////////////////////////////////////////////////////////////////
//HardwareExpert. Class connector HexEditorWidget <-> QHexEdit.
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

#include "hexeditorwidget.h"
#include "intelHexProvider/intelhexprovider.h"

HexEditorWidget::HexEditorWidget(QWidget *parent) :
    QHexEdit(parent)
{
connect(this,SIGNAL(currentAddressChanged(int)),SLOT(updateTextBar(int)));
connect(this,SIGNAL(dataChanged()),SLOT(on_dataChanged()));
lbl = NULL;
changed = false;
}

bool HexEditorWidget::loadFromFile(QString filename)
{
QFile f (filename);
if(!f.open(QFile::ReadOnly))return false;

setData(f.readAll());

f.close();


return true;
}

bool HexEditorWidget::loadFromHexFile(QString filename)
{
QByteArray d;

IntelHexProvider p;
p.loadFromHexFile(filename,d);

setData(d);

return true;
}

bool HexEditorWidget::saveToFile(QString filename)
{
    Q_UNUSED(filename);
    return false ;
}

void HexEditorWidget::updateTextBar(int addr)
{
    QString adr; adr.setNum(addr,16); adr = adr.toUpper();
    while(adr.length()<8)
    {
        adr.prepend("0");
    }


    QString val; val.setNum(this->data()[addr],16); val = val.toUpper();
    if(val.length()<2)val.prepend("0");
    val = val.mid(val.length()-2,2);

    QString bval; bval.setNum(this->data()[addr],2);
    while(bval.length()<8)bval.prepend("0");

    if(bval.length()>8)
    {
        bval = bval.mid(bval.length()-8,8);
    }
    lbl->setText("Address: "+adr+" Value: "+val+" Binary: "+bval);
}

void HexEditorWidget::setLineText(QLabel * label)
{
    lbl=label;
    lbl->setFont(QFont("Courier", 10));
    connect(this,SIGNAL(updateText(QString)),lbl,SLOT(setText(QString)));
}

void HexEditorWidget::on_dataChanged()
{
    changed = true;
}

bool HexEditorWidget::isChanged()
{
    return changed;
}

void HexEditorWidget::saved()
{
    changed = false;
}
