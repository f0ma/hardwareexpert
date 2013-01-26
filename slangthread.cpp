///////////////////////////////////////////////////////////////////////
//HardwareExpert. Thread container for S-Lang engine.
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

#include "slangthread.h"
#include <cstdlib>


SLangThread::SLangThread ( QObject * parent ) : QThread(parent)
{
code = NULL;
foo = NULL;
}

void SLangThread::setScript(QString s)
{
    if(code!=NULL)delete code;
    if(foo!=NULL)delete foo;


    
    code=new char[s.toUtf8().size()+1];
    QByteArray a = s.toUtf8();
    for(int i=0;i<s.toUtf8().size();i++)code[i]=a[i];
    code[s.toUtf8().size()]='\0';
    foo=NULL;

}

void SLangThread::setFunction(QString s)
{
    if(code!=NULL)delete code;
    if(foo!=NULL)delete foo;

    foo=new char[s.toUtf8().size()+1];
    for(int i=0;i<s.toUtf8().size();i++)foo[i]=s.toUtf8()[i];
    foo[s.toUtf8().size()]='\0';

    code=NULL;
}


void SLangThread::run()
{


    if(foo!=NULL)
    {
            SLang_execute_function(foo);
    }
    if(code!=NULL)
    {
        SLang_load_string(code);
    }
}
