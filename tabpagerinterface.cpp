///////////////////////////////////////////////////////////////////////
//HardwareExpert. Hex tab user interface.
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

#include "tabpagerinterface.h"
#include "mainwindow.h"

TabPagerInterface::TabPagerInterface()
{
    tw=NULL;
}

void TabPagerInterface::setParentWidget(QWidget * apwt)
{
    pwt=apwt;
}

void TabPagerInterface::addPage(QByteArray arr,QString text)
{
    emit addPageSignal(arr,text);

}


void TabPagerInterface::setCurrentPage(QByteArray * arr)
{
    if(tw->count()>0)
    {
    emit overwritePageSignal( arr);
    }
}


QByteArray TabPagerInterface::getCurrentPage()
{
    if(tw->count()>0)
    {
        return static_cast <HexEditorWidget *> (tw->currentWidget())->data();
    }
    return "";
}

void TabPagerInterface::deleteCurrentPage()
{

    if(tw->count()>0)
    {
        delete tw->currentWidget();
        tw->removeTab(tw->currentIndex());
    }
}

void TabPagerInterface::setTabWidget(QTabWidget * atw)
{
    tw=atw;
}

