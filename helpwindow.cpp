///////////////////////////////////////////////////////////////////////
//HardwareExpert. Help window.
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

#include "helpwindow.h"
#include "ui_helpwindow.h"

#include "settings.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
    textBrowser = new QTextBrowser(this);


    this->setWindowFlags(Qt::Tool|Qt::CustomizeWindowHint|
                         Qt::WindowTitleHint|
                         Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint);

    QStringList sl;

    use_settings;

    sl << settings.value("helpPath","").toString();
    sl << QCoreApplication::applicationDirPath() + "/doc";
    sl << QCoreApplication::applicationDirPath() + "/help";

    textBrowser->setSearchPaths ( sl );

    textBrowser->setSource(QUrl("index.html"));

    QPushButton * pbnext = new QPushButton(">>",this);
    QPushButton * pbpred = new QPushButton("<<",this);
    QPushButton * pbhome = new QPushButton(tr("Content"),this);


    QHBoxLayout * blay = new QHBoxLayout();
    blay->addWidget(pbpred);
    blay->addWidget(pbnext);
    blay->addStretch(10);
    blay->addWidget(pbhome);

    connect(pbpred,SIGNAL(clicked()),textBrowser,SLOT(backward()));
    connect(pbnext,SIGNAL(clicked()),textBrowser,SLOT(forward()));
    connect(pbhome,SIGNAL(clicked()),this,SLOT(openIndex()));


    QVBoxLayout * lay = new QVBoxLayout();

   lay->addLayout(blay);
   lay->addWidget(textBrowser);

   this->setLayout(lay);



    ui->setupUi(this);
}

void HelpWindow::openIndex()
{
        textBrowser->setSource(QUrl("index.html"));
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
