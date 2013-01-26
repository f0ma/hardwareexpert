///////////////////////////////////////////////////////////////////////
//HardwareExpert. Class connector Console <-> QPlaneTextEdit.
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

#include "planetextconsoleinterface.h"
#include "settings.h"

PlaneTextConsoleInterface::PlaneTextConsoleInterface()
{
    progVal=0;
    pb=NULL;
    te=NULL;

    cons = new QVector<QString>;

    upd = new QTimer(this);
    upd->setSingleShot(false);
    upd->setInterval(500);

    connect(upd,SIGNAL(timeout()),this,SLOT(updateTimer()));

    change=false;

    upd->start();



}

void PlaneTextConsoleInterface::updateTimer()
{
    if(!change)return;

    emit setProgbarValue(progVal);
    QString s;

    use_settings;

    int len = 0xFFFFFF;

    cons_mutex.lock();
    if (settings.value("trunkLog",0).toInt()!=0)
    {
    len = settings.value("trunkLen",100).toInt();
    if (cons->size()>=len) cons->remove(0,cons->size()-len);
    }


    for(int i=0;i<cons->size() && i<len ;i++)
    {
        s+=cons->at(i);
    }

    emit clearConsole();
    emit appendToConsole(s);


    cons_mutex.unlock();

    change=false;
}

void PlaneTextConsoleInterface::setProgressBar(QProgressBar * p)
{
    pb=p;

    connect(this,SIGNAL(setProgbarValue(int)),pb,SLOT(setValue(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(setProgbarMax(int)),pb,SLOT(setMaximum(int)),Qt::QueuedConnection);

}

void PlaneTextConsoleInterface::setPlaneTextEdit(QPlainTextEdit * t)
{
    te=t;

    connect(this,SIGNAL(appendToConsole(QString)),te,SLOT(appendPlainText(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(clearConsole()),te,SLOT(clear()),Qt::QueuedConnection);
    connect(this,SIGNAL(setConsole(QString)),te,SLOT(setPlainText(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(quest(QString,QString,int*)),this->parent(),SLOT(questUser(QString,QString,int*)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(callOptionDialog(QVector<ConsoleInterface::optionDialogEntry>*,bool*)),this->parent(),SLOT(callOptionDialog(QVector<ConsoleInterface::optionDialogEntry>*,bool*)),Qt::BlockingQueuedConnection);

}

void PlaneTextConsoleInterface::print(QString s)
{

cons_mutex.lock();
    cons->append(s);
   change=true;
   cons_mutex.unlock();

}

void PlaneTextConsoleInterface::clear()
{
   emit clearConsole();
   cons_mutex.lock();
   cons->clear();
   cons_mutex.unlock();
}

void PlaneTextConsoleInterface::setProgressValue(int s)
{
    progVal=s;
    change=true;
}

void PlaneTextConsoleInterface::setProgressMax(int s)
{
    emit setProgbarMax(s);
}

void PlaneTextConsoleInterface::setProgressWait()
{
    emit setProgbarValue(-1);
}

void PlaneTextConsoleInterface::clearProgress()
{
    setProgressValue(0);
    emit setProgbarValue(0);
}

void PlaneTextConsoleInterface::rmline()
{
cons_mutex.lock();
    cons->remove(cons->size()-1);
cons_mutex.unlock();
}

int PlaneTextConsoleInterface::quest(QString title,QString text)
{
    int r;
    emit quest(title,text,&r);
    return r;
}

bool PlaneTextConsoleInterface::optionDialog(QVector<optionDialogEntry> * options)
{
    bool r;
    emit callOptionDialog (options,&r);
    return r;
}
