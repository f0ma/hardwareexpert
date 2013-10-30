///////////////////////////////////////////////////////////////////////
//HardwareExpert. Search window.
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

#include "searchdialog.h"
#include "ui_searchdialog.h"
#include <QProgressDialog>

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);


    QRegExp rx("([A-Fa-f0-9][A-Fa-f0-9])+");
    hexValidator = new QRegExpValidator(rx, this);

    ui->lvResults->setModel(&model_results);

    results << tr("No results");
    model_results.setStringList(results);

}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_rbAsciiMode_clicked()
{
    ui->leSearch->setValidator(NULL);
}

void SearchDialog::on_rbHexMode_clicked()
{
    ui->leSearch->setValidator(hexValidator);
}

void SearchDialog::on_pbSearch_clicked()
{

    QByteArray data = editor->data();
    QVector <int> resultsval;
    QByteArray search;

    if(ui->rbAsciiMode->isChecked())
    {
        search= ui->leSearch->text().toAscii();
    }

    if(ui->rbHexMode->isChecked())
    {

        search = search.fromHex(ui->leSearch->text().toAscii());
    }

if(search.size()==0) return;

QProgressDialog * progress = new QProgressDialog(tr("Search..."), tr("Cancel"), 0, data.size(), this);
progress->setWindowModality(Qt::WindowModal);
progress->show();

    for(int i=0;i<data.size();i++)
    {
        if (i % 1000 == 0 )
        {
            progress->setValue(i);
            QCoreApplication::processEvents ();
            if (progress->wasCanceled()) break;
        }

        int j=0;

        for(;j<search.size();j++,i++)
        {
            if(data.at(i) != search.at(j)) break;
        }

        if(j==search.size())
        {
            resultsval.append(i-search.size());
        }

        i = i-j;
    }

    int r;

    results.clear();
    model_results.setStringList(results);

    if(resultsval.size()==0)
    {
        results << tr("No results");

        QByteArray empty;
        editor->setExternalHighlight(empty);
        model_results.setStringList(results);
    }
    else
    {
        QByteArray hl;
        hl.fill('\0',data.size());
        foreach(r,resultsval)
        {
            for(int i = r;i<search.size()+r;i++)
                hl[i]='S';

        QString s;
        s.setNum(r,16);
        results << "0x" + s.toUpper();
        }

        model_results.setStringList(results);
        editor->setExternalHighlight(hl);
    }

    progress->close();
    delete progress;
}

void SearchDialog::setText(QString s)
{
    ui->lTabName->setText(s);
}

void SearchDialog::setHexEditor(HexEditorWidget * h)
{
    results.clear();
    results << tr("No results");
    model_results.setStringList(results);

    QByteArray empty;
    h->setExternalHighlight(empty);
    editor = h;
}

void SearchDialog::closeEvent(QCloseEvent *event)
 {
    QByteArray empty;
    editor->setExternalHighlight(empty);

    event->accept();
 }

void SearchDialog::on_lvResults_clicked(const QModelIndex &index)
{
    if (index.data().toString()!=tr("No results"))
        editor->gotoAddress(index.data().toString().toInt(NULL,16));
}
