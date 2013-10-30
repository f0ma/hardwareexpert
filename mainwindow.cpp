///////////////////////////////////////////////////////////////////////
//HardwareExpert. Main window.
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

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "intelHexProvider/intelhexprovider.h"
#include <slang.h>
#include "aboutwindow.h"
#include "searchdialog.h"


#include "settings.h"


//TODO Cursor to last log line

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    itptr=new SLangWrapper();

    waitImgs << ":/progress/prog0.png"
             << ":/progress/prog1.png"
             << ":/progress/prog2.png"
             << ":/progress/prog3.png"
             << ":/progress/prog4.png"
             << ":/progress/prog5.png"
             << ":/progress/prog6.png"
             << ":/progress/prog7.png";



    waitTimer = new QTimer(this);
    waitTimer->setInterval(100);
    waitTimer->setSingleShot(false);

    connect(waitTimer,SIGNAL(timeout()),this,SLOT(onProgressTimerTick()));


    connect(itptr,SIGNAL(executionStarted()),this,SLOT(onScriptRunning()));
    connect(itptr,SIGNAL(executionEnded()),this,SLOT(onScriptStoped()));
    connect(ui->lwApi,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_pbExecute_clicked()));

    ce = new CodeEditor(this);

    ui->wtPanel->insertTab(1,ce,tr("Code"));
    ui->wtPanel->setCurrentIndex(0);

    codeFileManager = new CodeFile(this);

    connect(codeFileManager,SIGNAL(setFilenameIndicator(QString)),this,SLOT(onFilenameIndicatorChanged(QString)));


    PlaneTextConsoleInterface * cis = new PlaneTextConsoleInterface();

    cis->setParent(this);

    cis->setPlaneTextEdit(ui->consoleView);
    cis->setProgressBar(ui->progressBar);

    itptr->setConsoleInterface(cis);
    ci=cis;


    hfm = new HexFilesMenager(this);

    hfm->setTabWidget(ui->twHex);
    hfm->setInfoLine(ui->hexInfoLine);

    itptr->setMemoryInterface(hfm);

    imp = new ImportExportMenager(this);
    imp->setHexFilesMenager(hfm);
    imp->setConsoleInterfaceMenager(ci);

    onScriptStoped();

    driverWindow = new DriverDialog(this);
    driverWindow->setModal(true);

    cis->print("Hardware configuration:\n");
    cis->print("COM port: "+driverWindow->getCurrentComPort()->description()+"\n");
    cis->print("LPT port: "+driverWindow->getCurrentLptPort()->description()+"\n");
    cis->print("I2C port: "+driverWindow->getCurrentI2cPort()->description()+"\n");

    optionWindow = new OptionsDialog(this);
    optionWindow->setModal(true);


    codeFileManager->setCodeEditor(ce);
    codeFileManager->setSelectTreeWidget(ui->twScriptCatalog);
    connect(optionWindow,SIGNAL(updateScriptLibrary(QStringList)),codeFileManager,SLOT(updateLibrary(QStringList)));
    optionWindow->sendPathList();


    //connect(ti,SIGNAL(addPageSignal(QByteArray,QString)),this,SLOT(addNewHexPage(QByteArray,QString)));
    //connect(ti,SIGNAL(overwritePageSignal(QByteArray*)),this,SLOT(overwriteHexPage(QByteArray*)),Qt::QueuedConnection);

    connect(hfm,SIGNAL(enableFilePrcessing(bool)),ui->menuHex,SLOT(setEnabled(bool)));

    connect(ce,SIGNAL(textChanged()),this,SLOT(scriptChanged()));


    use_settings;



    if (settings.value("saveGui",0).toInt()==2)
    {

    settings.beginGroup("Window");
    this->setGeometry(settings.value("Geometry",this->geometry()).toRect());
    this->restoreState(settings.value("State",this->saveState()).toByteArray());
    settings.endGroup();

    }




    installEventFilter(this);


    helpWindow = new HelpWindow(this);
    emit hexPageCountChange();


}



void MainWindow::callOptionDialog (QVector<ConsoleInterface::optionDialogEntry> * options,bool * ret)
{
if (options->isEmpty()) return;

QVectorIterator<ConsoleInterface::optionDialogEntry> it(*options);

QDialog * d = new QDialog(this);

d->setModal(true);

QVBoxLayout * dbl = new QVBoxLayout();
d->setLayout(dbl);

QList <QWidget * > ents;

QHBoxLayout * sl;
QLabel * l;
QLineEdit * lew;
QCheckBox * chw;
QComboBox * cbl;

for(int i=0;i< (*options).size();i++)
{
sl = new QHBoxLayout();
l = new QLabel(d);
l->setText(it.next().text);
sl->addWidget(l);

if((*options)[i].type == ConsoleInterface::OPT_DEC)
{
    lew = new QLineEdit(d);
    lew->setValidator(new QIntValidator( d ));
    QString s;
    s.setNum((*options)[i].valueInt);
    lew->setText(s);
    sl->addWidget(lew);
    ents.append(lew);
}

if((*options)[i].type == ConsoleInterface::OPT_HEX)
{
    lew = new QLineEdit(d);
    lew->setValidator(new QRegExpValidator(QRegExp("([A-Fa-f0-9][A-Fa-f0-9])+"),d));
    QString s;
    s.setNum((*options)[i].valueInt,16);
    lew->setText(s);
    sl->addWidget(lew);
    ents.append(lew);
}

if((*options)[i].type == ConsoleInterface::OPT_STR)
{
    lew = new QLineEdit(d);
    lew->setValidator(NULL);
    QString s = (*options)[i].valueStr;
    lew->setText(s);
    sl->addWidget(lew);
    ents.append(lew);
}


if((*options)[i].type == ConsoleInterface::OPT_CHK)
{
    chw = new QCheckBox(d);
    chw->setChecked((*options)[i].valueInt == 0 ? false : true);
    sl->addWidget(chw);
    ents.append(chw);
}


if((*options)[i].type == ConsoleInterface::OPT_LIST)
{
cbl = new QComboBox(d);
cbl->addItems((*options)[i].variants);
sl->addWidget(cbl);
ents.append(cbl);
}




dbl->addLayout(sl);


}


QHBoxLayout * hbl = new QHBoxLayout(d);

QPushButton * obOK = new QPushButton(tr("OK"),d);
QPushButton * obCancel = new QPushButton(tr("Cancel"),d);
QPushButton * obAbort = new QPushButton(tr("Abort"),d);

connect(obOK,SIGNAL(clicked()),d,SLOT(accept()));
connect(obCancel,SIGNAL(clicked()),d,SLOT(reject()));
connect(obAbort,SIGNAL(clicked()),d,SLOT(reject()));
connect(obAbort,SIGNAL(clicked()),this,SLOT(on_stopButton_clicked()),Qt::QueuedConnection);

hbl->addStretch(10);
hbl->addWidget(obOK);
hbl->addWidget(obCancel);
hbl->addWidget(obAbort);

dbl->addLayout(hbl);


if(d->exec()==QDialog::Accepted)
{

    for(int i=0;i< (*options).size();i++)
    {
        if((*options)[i].type == ConsoleInterface::OPT_DEC)
        {
            (*options)[i].valueInt  = dynamic_cast<QLineEdit*>(ents[i])->text().toInt();
        }

        if((*options)[i].type == ConsoleInterface::OPT_HEX)
        {
            (*options)[i].valueInt  = dynamic_cast<QLineEdit*>(ents[i])->text().toInt(0,16);
        }

        if((*options)[i].type == ConsoleInterface::OPT_STR)
        {
            QString t =  dynamic_cast<QLineEdit*>(ents[i])->text();
            (*options)[i].valueStr  = t;
        }


        if((*options)[i].type == ConsoleInterface::OPT_CHK)
        {
            (*options)[i].valueInt  = dynamic_cast<QCheckBox*>(ents[i])->isChecked();
        }


        if((*options)[i].type == ConsoleInterface::OPT_LIST)
        {
            (*options)[i].valueInt  = dynamic_cast<QComboBox*>(ents[i])->currentIndex();
        }
    }

*ret =true;
}
else
{
*ret =false;
}



delete d;
}

ConsoleInterface * MainWindow::getConsole()
{
    return ci;
}

void MainWindow::onScriptRunning()
{
        ui->pbExecute->setEnabled(false);
        ui->runButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->menuOptions->setEnabled(false);
        ui->lwApi->setEnabled(false);
        waitTimer->start();
        hfm->lockEditor();
}

void MainWindow::onScriptStoped()
{
        waitTimer->stop();
        ui->waitLabel->setText(" ");

        ui->pbExecute->setEnabled(true);
        ui->runButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->menuOptions->setEnabled(true);
        ui->lwApi->setEnabled(true);
        hfm->unLockEditor();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::on_runButton_clicked()
{
    ui->consoleView->setFocus();

    itptr->setComPortInterface(driverWindow->getCurrentComPort());
    itptr->setLptPortInterface(driverWindow->getCurrentLptPort());
    itptr->setI2cPortInterface(driverWindow->getCurrentI2cPort());

    itptr->setScriptPath(codeFileManager->currentCodePath());

    itptr->run(ce->toPlainText());


    QMap<QString,QString> api = itptr->getApi();
    ui->lwApi->clear();

    for(int i=0;i<api.count();i++)
    {
        QListWidgetItem *item = new QListWidgetItem(api.values().operator [](i),ui->lwApi);
        item->setIcon(QIcon(":/icons/icon_chip.png"));

    }

    ui->pbExecute->setEnabled(true);

    if(api.count()!=0 && !codeFileManager->codeChanged())ui->wtPanel->setCurrentIndex(2);

    ci->print(tr("\nCompilation ended.\n"));

}

void MainWindow::on_stopButton_clicked()
{
    itptr->stop();
}

void MainWindow::on_actionOpen_Hex_triggered()
{    itptr->setScriptPath(codeFileManager->currentCodePath());
     use_settings;

     settings.beginGroup("FileDialogs");
     QString path = settings.value("BinDialogPath","").toString();
     settings.endGroup();

     QString s = QFileDialog::getOpenFileName ( this, tr("Open Bin file"),path,tr("Binary files (*.bin);;All files (*.*)"));

    if(s != "")
    {
            hfm->loadFromBinary(s);
            QFileInfo fi(s);
            settings.beginGroup("FileDialogs");
            settings.setValue("BinDialogPath",fi.absolutePath());
            settings.endGroup();
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpen_Code_triggered()
{

    use_settings;

    if(!codeFileManager->openFromFile()) return;

    int ret;

    if(settings.value("executeNow",0).toInt()==2)
    {

    QMessageBox msgBox;
    msgBox.setText(tr("Code loaded"));
    msgBox.setInformativeText(tr("Execute now?"));
    msgBox.setStandardButtons( QMessageBox::No | QMessageBox::Yes );
    msgBox.setDefaultButton(QMessageBox::Yes);
    ret = msgBox.exec();
    }
    else ret = QMessageBox::Yes;

    if(ret == QMessageBox::No)
    {
        ui->wtPanel->setCurrentIndex(1);

    }
    if(ret == QMessageBox::Yes)
    {
        runCodeOnLoad();
    }


}

void MainWindow::runCodeOnLoad()
{
    this->on_runButton_clicked();
    ui->wtPanel->setCurrentIndex(2);
}

void MainWindow::on_actionNew_code_triggered()
{
    codeFileManager->newFile();
}

void MainWindow::on_actionSave_code_as_triggered()
{
    codeFileManager->saveToFileAs();
}

void MainWindow::on_actionSave_code_triggered()
{
    codeFileManager->saveToFile();
}

void MainWindow::on_actionSave_hex_triggered()
{
    if(hfm->pageCount()==0)return;

    use_settings;

    settings.beginGroup("FileDialogs");
    QString path = settings.value("BinDialogPath","").toString();
    settings.endGroup();

    QString fname = ui->twHex->tabText(ui->twHex->currentIndex());
    if (fname.endsWith("*")) fname = fname.mid(0,fname.length()-1);

    QString s = QFileDialog::getSaveFileName(this, tr("Save binary"),path + "/" + fname,tr("Binary files (*.bin);;All files (*.*)"));

    if(s=="")return;

     if(!hfm->saveToBinary(s)) QMessageBox::warning(this,tr("File save error"),tr("Can't save file"));
}

void MainWindow::on_pbExecute_clicked()
{
    itptr->setComPortInterface(driverWindow->getCurrentComPort());
    itptr->setLptPortInterface(driverWindow->getCurrentLptPort());
    itptr->setI2cPortInterface(driverWindow->getCurrentI2cPort());

    itptr->setScriptPath(codeFileManager->currentCodePath());

    if(ui->lwApi->currentRow()!=-1)
    {
        itptr->execute(itptr->getApi().key(ui->lwApi->currentItem()->text()));
    }

}




void MainWindow::on_pbClearLog_clicked()
{
    ui->consoleView->clear();
    ci->clear();
    ci->clearProgress();

}

void MainWindow::on_pbSaveLog_clicked()
{
    use_settings;

    settings.beginGroup("FileDialogs");
    QString path = settings.value("BinDialogPath","").toString();
    settings.endGroup();


    QString s = QFileDialog::getSaveFileName(this, tr("Save log"),path,tr("Log (*.log);;All files (*.*)"));
    if(s.isEmpty())return;
    QFile f (s);
    f.open(QFile::WriteOnly);
    if(!f.isOpen())
    {
        QMessageBox::warning(this,tr("File save error"),tr("Can't save file"));
        return;
    }
    f.write(ui->consoleView->toPlainText().toUtf8());
    f.close();
}

void MainWindow::on_actionAbout_triggered()
{

AboutWindow * abw = new AboutWindow(this);

abw->setWindowModality(Qt::WindowModal);

abw->show();

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,tr("About Qt"));
}



void MainWindow::on_actionHelp_triggered()
{
    helpWindow->show();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type()==QEvent::Close && obj == this)
    {
            if(codeFileManager->saveUnsaved() && hfm->allTabCloseRequested())
            {
                use_settings;

                settings.beginGroup("Window");
                settings.setValue("State",this->saveState());
                settings.setValue("Geometry",this->geometry());
                settings.endGroup();


                ev->accept();
                return true;
            }
            ev->ignore();
            return true;

    }
    return QWidget::eventFilter(obj,ev);
}



void MainWindow::questUser(QString title,QString txt, int * result)
{
    int q =QMessageBox::question(this,title,txt,QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
    if(q==QMessageBox::Yes)(*result) = 1;
    if(q==QMessageBox::No)(*result) = 0;
    if(q==QMessageBox::Abort)(*result) = -1;
}


void MainWindow::on_actionOptions_triggered()
{
    optionWindow->show();
}

void MainWindow::on_actionDrivers_triggered()
{
    driverWindow->show();
}

void MainWindow::scriptChanged()
{
    ui->pbExecute->setEnabled(false);
}

void MainWindow::onProgressTimerTick()
{

    QString s = waitImgs.first();
    QPixmap pixmap (s);
    ui->waitLabel->setPixmap(pixmap);
    waitImgs.pop_front();
    waitImgs.push_back(s);
}


void MainWindow::on_actionSearch_String_triggered()
{
    HexEditorWidget* v = dynamic_cast<HexEditorWidget*>(ui->twHex->currentWidget());
    if (v==NULL)return;
    QByteArray br = v->data();
    QByteArray akk;
    int lastAdr=0;

    QProgressDialog * progress = new QProgressDialog(tr("Search..."), tr("Cancel"), 0, br.size(), this);
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    for(int i=0;i<=br.count();i++)
    {
        if (i % 1000 == 0 )
        {
            progress->setValue(i);
            QCoreApplication::processEvents ();
            if (progress->wasCanceled()) break;
        }

        unsigned char c = br[i];
        if( c >= 0x20 && c <= 0x7E)
        {
            if(akk.count()==0)lastAdr=i;
            akk.append(c);

        }
        else
        {
            if (akk.length()>3)
            {
                QString s;
                s.setNum(lastAdr,16);
                ui->consoleView->appendPlainText(s.toUpper() + " : \""+akk+"\"");
            }
            akk.clear();
        }
    }

    progress->close();
    delete progress;
}



void MainWindow::on_actionSearch_triggered()
{
    SearchDialog * sl = new SearchDialog(this);
    HexEditorWidget* v = dynamic_cast<HexEditorWidget*>(ui->twHex->currentWidget());

    sl->setText(ui->twHex->tabText(ui->twHex->currentIndex()));
    sl->setHexEditor(v);
    sl->show();
}

void MainWindow::on_actionDiff_triggered()
{
    CompairDialog * cd = new CompairDialog(this);
    for(int i=0;i<ui->twHex->count();i++)
    {
        cd->addEditor(dynamic_cast<HexEditorWidget*>(ui->twHex->widget(i)),ui->twHex->tabText(i));
    }
    cd->show();
}

void MainWindow::on_actionNew_hex_triggered()
{
    HexEditorWidget * w = new HexEditorWidget(this);
    w->setLineText(ui->hexInfoLine);
    w->setData(QByteArray());

    QStringList tnames;
    QString tline="NewFile";

    for(int i=0;i<ui->twHex->count();i++)
        tnames.append(ui->twHex->tabText(i));

    int cn =1;

    while(tnames.contains(tline))
    {
        tline = "NewFile-" +  QString("%1").arg(cn);
        cn++;
    }

    ui->twHex->addTab(w,tline);
    ui->twHex->setCurrentIndex(ui->twHex->count()-1);
    emit hexPageCountChange();

}


void MainWindow::onFilenameIndicatorChanged(QString s)
{
    if (s.isEmpty()) this->setWindowTitle("HardwareExpert");
    else
    {
        this->setWindowTitle("[" + s.split("/").last().split("\\").last() + "] - HardwareExpert");
        if(s.lastIndexOf("*") != s.length()-1) ci->print(tr("\nLoad script from: ") + s+"\n");
    }
}

void MainWindow::on_actionClose_all_binary_triggered()
{
    hfm->allTabCloseRequested();
}

void MainWindow::on_actionFind_all_text_string_16_bit_triggered()
{
    HexEditorWidget* v = dynamic_cast<HexEditorWidget*>(ui->twHex->currentWidget());
    if (v==NULL)return;
    QByteArray br = v->data();
    QByteArray akk;
    int lastAdr=0;

    QProgressDialog * progress = new QProgressDialog(tr("Search..."), tr("Cancel"), 0, br.size(), this);
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    for(int i=0;i<=br.count()-1;i=i+2)
    {
        if (i % 1000 == 0 )
        {
            progress->setValue(i);
            QCoreApplication::processEvents ();
            if (progress->wasCanceled()) break;
        }

        unsigned char c = br[i];
        unsigned char d = br[i+1];

        bool mached = false;

        if( d >= 0x20 && d <= 0x7E && c == 0x00)
        {
            if(akk.count()==0)lastAdr=i;
            akk.append(d);
            mached = true;

        }

        if( d >= 0x20 && d <= 0x7E && c >= 0x20 && c <= 0x7E)
        {
            if(akk.count()==0)lastAdr=i;
            akk.append(d);
            akk.append(c);
            mached = true;

        }

        if(!mached)
        {
            if (akk.length()>3)
            {
                QString s;
                s.setNum(lastAdr,16);
                ui->consoleView->appendPlainText(s.toUpper() + " : \""+akk+"\"");
            }
            akk.clear();
        }
    }

    progress->close();
    delete progress;
}

void MainWindow::on_actionImportFile_triggered()
{
    use_settings;

    settings.beginGroup("FileDialogs");
    QString path = settings.value("BinDialogPath","").toString();
    settings.endGroup();

    QString format;

    QString s = QFileDialog::getOpenFileName(this, tr("Import file"),path,
                                             imp->getFormatImportList(),&format);
    if(s != "")
    {

        if(!imp->importFile(s,format)) QMessageBox::warning(this,tr("File load error"),tr("Can't import file"));
        else ci->print(tr("File: ")+s+tr(" loaded.\n"));

        emit hexPageCountChange();


    QFileInfo fi(s);
    settings.beginGroup("FileDialogs");
    settings.setValue("BinDialogPath",fi.absolutePath());
    settings.endGroup();
    }
}

void MainWindow::on_actionExportFile_triggered()
{
    if(ui->twHex->count()==0)return;

    use_settings;

    settings.beginGroup("FileDialogs");
    QString path = settings.value("BinDialogPath","").toString();
    settings.endGroup();

    QString format;

    QString s = QFileDialog::getSaveFileName(this, tr("Export file"),path,
                                             imp->getFormatExportList(),&format);



    if(s=="")return;



    if(!imp->exportFile(s,format)) QMessageBox::warning(this,tr("File save error"),tr("Can't save file"));

}
