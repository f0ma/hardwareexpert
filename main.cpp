///////////////////////////////////////////////////////////////////////
//HardwareExpert. Entry point module.
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

#include <QtGui/QApplication>
#include <QTextCodec>
#include <QLibraryInfo>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    Q_INIT_RESOURCE(resources);

    QTranslator qtTranslator;
    if(!qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    qtTranslator.load("qt_" + QLocale::system().name());

    a.installTranslator(&qtTranslator);

    QTranslator translator;
    translator.load("hardwareexpert_" + QLocale::system().name());
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
