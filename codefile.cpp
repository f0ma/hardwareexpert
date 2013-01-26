///////////////////////////////////////////////////////////////////////
//HardwareExpert. Scripts holder.
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

#include "codefile.h"

#include "mainwindow.h"
#include "settings.h"

CodeFile::CodeFile(QObject *parent) :
    QObject(parent)
{
    codeEditor=NULL;
    treeWidget=NULL;
    currentCodeChanged=false;
}

void CodeFile::setCodeEditor(QPlainTextEdit * ple)
{
    codeEditor = ple;
    connect(ple,SIGNAL(textChanged()),
            this,SLOT(planeEdittextChanged()));
}

void CodeFile::setSelectTreeWidget(QTreeWidget * tw)
{
    treeWidget=tw;
    connect(tw,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(treeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
}

void CodeFile::updateLibrary(QStringList paths)
{
    sourceDirectoryList=paths;
    updateTree();
}

void CodeFile::updateTree()
{
    codeCategory.clear();
    codeFiles.clear();

    QString path;

    foreach (path,sourceDirectoryList)
    {
        QDir dirfile(path);
        QStringList ftr;
        ftr << "*.slh";
        QStringList filelist = dirfile.entryList (ftr);

        QString fn;

        foreach (fn,filelist)
        {
            QFile f (path+"/"+fn);
            f.open(QFile::ReadOnly);
            if(!f.isOpen()) continue;
            QString line;

            QTextCodec * tk= QTextCodec::codecForName("UTF8");

            line = tk->toUnicode(f.readLine());

            if(line.length()>0 && line[0]=='%')
            {
                QString txtline = line.mid(1,line.length()-1);

                QStringList txtarray = txtline.split("\\");

                if(txtarray.length()!=3)continue;

                CodeFileStruct l;

                l.category=txtarray[0];

                l.name=txtarray[1];
                l.desc=txtarray[2].replace("\n","");
                l.fileName=fn;
                l.filePath=path+"/"+fn;

                codeFiles.append(l);

                codeCategory.append(txtarray[0]);

            }
        }

    }

    treeWidget->clear();

    QString cat;
    CodeFileStruct file;
    int s =0;

    codeCategory.removeDuplicates();
    codeCategory.sort();

    foreach (cat,codeCategory)
    {
    QTreeWidgetItem * w =new QTreeWidgetItem();
    w->setText(0,cat);

    foreach (file,codeFiles)
    {
        if(file.category==cat)
        {
        QTreeWidgetItem * m =new QTreeWidgetItem();
        m->setText(0,file.name);
        m->setText(1,file.desc);
        m->setText(2,file.fileName);

        w->addChild(m);
        }
    }

    treeWidget->insertTopLevelItem(s,w);
    s++;
    }

    treeWidget->expandAll();
    treeWidget->setAllColumnsShowFocus(true);
    treeWidget->resizeColumnToContents(0);
    treeWidget->resizeColumnToContents(1);
    treeWidget->resizeColumnToContents(2);

}

bool CodeFile::saveUnsaved()
{
    if(currentCodeChanged==false) return true;

    if(codeFileName=="" && codeEditor->toPlainText().length()==0)  return true;

        QMessageBox msgBox;
        msgBox.setText(tr("Code has been modified"));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Cancel) return false;
        if(ret == QMessageBox::Save)
             {
                 saveToFile();
                 if(currentCodeChanged==false)return true;
                 else return false;
             }
        if(ret == QMessageBox::Discard)return true;

    return false;

}

void CodeFile::newFile()
{
    if(saveUnsaved())
    {
        codeEditor->clear();
        codeFileName="";
        currentCodeChanged=true;
        emit setFilenameIndicator("");
    }
}

bool CodeFile::openFromFile()
{
    if(saveUnsaved())
    {

        use_settings;

        settings.beginGroup("FileDialogs");
        QString path = settings.value("ScriptDialogPath","").toString();
        settings.endGroup();


        QString s = QFileDialog::getOpenFileName(static_cast <QWidget*>(this->parent()), tr("Open code"),path,
                                                 tr("S-Lang for HardwareExpert (*.slh);;All files (*.*)"));
        if(s.isEmpty())return false;



        QFileInfo fi(s);
        settings.beginGroup("FileDialogs");
        settings.setValue("ScriptDialogPath",fi.absolutePath());
        settings.endGroup();



        QFile f (s);
        f.open(QFile::ReadOnly);
        if(!f.isOpen())
        {
            QMessageBox::warning(static_cast <QWidget*>(this->parent()),tr("File open error"),tr("Can't open file"));
            return false;
        }

        QTextCodec * tk= QTextCodec::codecForName("UTF8");

        codeEditor->setPlainText(tk->toUnicode(f.readAll()));

        f.close();

        codeFileName=s;
        currentCodeChanged=false;
        emit setFilenameIndicator(s);
        return true;

    }
    return false;
}

QString CodeFile::currentCodePath()
{
    QFileInfo d (codeFileName);
    return d.dir().path();
}

void CodeFile::saveToFile()
{
    if(codeFileName=="") saveToFileAs();

    else
        {
            QFile f (codeFileName);

            if(f.exists())
            {

            f.open(QFile::ReadOnly);

            if(!f.isOpen())
            {
                QMessageBox::warning(static_cast <QWidget*>(this->parent()),tr("File save error"),tr("Can't save file"));
                return;
            }

            use_settings;

            if(settings.value("useBackup",0).toInt()==2)
            {
            QFile bkpFile (codeFileName+"."+QDateTime::currentDateTime().toString("yyMMddhhmmss"));
            bkpFile.open(QFile::WriteOnly);

            if(bkpFile.isOpen())
            {
            bkpFile.write(f.readAll());
            bkpFile.close();
            }
            }

            f.close();

            }

            f.open(QFile::WriteOnly | QFile::Truncate);

            if(!f.isOpen())
            {
                QMessageBox::warning(static_cast <QWidget*>(this->parent()),tr("File save error"),tr("Can't save file"));
                return;
            }

            f.write(codeEditor->toPlainText().toUtf8());
            f.close();

        currentCodeChanged=false;
        emit setFilenameIndicator(codeFileName);
        }

}

void CodeFile::saveToFileAs()
{
    use_settings;

    settings.beginGroup("FileDialogs");
    QString path = settings.value("ScriptDialogPath","").toString();
    settings.endGroup();

    QString s = QFileDialog::getSaveFileName(static_cast <QWidget*>(this->parent()), tr("Save code"),path+"/"+codeFileName,
                                              tr("S-Lang for HardwareExpert (*.slh);;All files (*.*)"));
     if(s=="")return;
     if(s.indexOf('.')==-1)s+=".slh";

     codeFileName=s;

     saveToFile();
}


void CodeFile::treeWidgetItemDoubleClicked ( QTreeWidgetItem * item, int column )
{
    Q_UNUSED(column);

    if(item->columnCount()>1)
    {
        QString s = item->text(0);
        QString path;

        CodeFileStruct file;

        foreach (file,codeFiles)
        {
            if(file.name == s)
            {
            path = file.filePath;
            break;
            }
        }

        if(path=="")return;


        if(saveUnsaved())
        {
            if(s.isEmpty())return;
            QFile f (path);
            f.open(QFile::ReadOnly);
            if(!f.isOpen())
            {
                QMessageBox::warning(static_cast <QWidget*>(this->parent()),tr("File open error"),tr("Can't open file"));
                return;
            }

            QTextCodec * tk= QTextCodec::codecForName("UTF8");

            codeEditor->setPlainText(tk->toUnicode(f.readAll()));

            f.close();

            codeFileName=path;
            currentCodeChanged=false;
            emit setFilenameIndicator(codeFileName);
            static_cast<MainWindow*> (this->parent())->runCodeOnLoad();

        }
    }
}

void CodeFile::planeEdittextChanged ()
{
    currentCodeChanged=true;
    emit setFilenameIndicator(codeFileName+" *");
}

bool CodeFile::codeChanged()
{
    return currentCodeChanged;
}
