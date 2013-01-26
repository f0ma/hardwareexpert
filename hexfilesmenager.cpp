#include "hexfilesmenager.h"

HexFilesMenager::HexFilesMenager(QObject *parent) :
    QObject(parent)
{
    connect(this,SIGNAL(sig_addPage(QByteArray,QString)),SLOT(on_addPage(QByteArray,QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(sig_deleteCurrentPage()),SLOT(on_deleteCurrentPage()),Qt::QueuedConnection);
    connect(this,SIGNAL(sig_setCurrentPage(QByteArray*)),SLOT(on_setCurrentPage(QByteArray*)),Qt::QueuedConnection);
}

void HexFilesMenager::setInfoLine(QLabel * il)
{
    infoline=il;
}

void HexFilesMenager::addPage(QByteArray arr,QString text)
{
    emit sig_addPage(arr,text);
}

void HexFilesMenager::on_addPage(QByteArray arr,QString text)
{
    HexEditorWidget * w = new HexEditorWidget(dynamic_cast<QWidget*> (this->parent()));
    connect(w,SIGNAL(dataChanged()),this,SLOT(on_dataChanged()));
    w->setLineText(infoline);
    w->setData(arr);

    QStringList tnames;
    QString tline=text;

    for(int i=0;i<tw->count();i++)
        tnames.append(tw->tabText(i));

    int cn =1;

    while(tnames.contains(tline) || tnames.contains(tline+"*"))
    {
        tline = text + "-" +  QString("%1").arg(cn);
        cn++;
    }

    tw->addTab(w,tline);
    w->on_dataChanged();
    tw->setCurrentIndex(tw->count()-1);
    on_dataChanged();
}

void HexFilesMenager::setCurrentPage(QByteArray * arr)
{
    emit sig_setCurrentPage(arr);
}

void HexFilesMenager::on_setCurrentPage(QByteArray * arr)
{
    HexEditorWidget * e = static_cast <HexEditorWidget *>  (tw->currentWidget());
    e->setData(*arr);
    delete arr;
}

QByteArray HexFilesMenager::getCurrentPage()
{
    if(tw->count()>0)
    {
        return static_cast <HexEditorWidget *> (tw->currentWidget())->data();
    }
    return "";
}

void HexFilesMenager::deleteCurrentPage()
{
    emit sig_deleteCurrentPage();
}

void HexFilesMenager::on_deleteCurrentPage()
{
    if(tw->count()>0)tw->removeTab(tw->currentIndex());
}

void HexFilesMenager::setTabWidget(QTabWidget * atw)
{
    tw=atw;
    tw->setTabsClosable(true);
    connect(tw,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
}


bool HexFilesMenager::loadFromHex(QString fileName)
{
    QFileInfo fi (fileName);

    QByteArray d;

    IntelHexProvider p;
    p.loadFromHexFile(fileName,d);


    this->on_addPage(d,fi.fileName());
    static_cast <HexEditorWidget *> (tw->currentWidget())->saved();
    on_dataSaved();
    return true;
}

bool HexFilesMenager::saveToHex(QString fileName)
{
    if(fileName.indexOf('.')==-1)fileName+=".hex";

    QByteArray data = this->getCurrentPage();

    IntelHexProvider p;
    if (data.size()>0xFFFFFF)
    {
        p.setAddressing(IntelHexProvider::iHEX_AMODE_32bit);
        p.setDataRecordLength(IntelHexProvider::iHEX_DRMODE_16bit);
    }
    else
    {
        p.setAddressing(IntelHexProvider::iHEX_AMODE_16bit);
        p.setDataRecordLength(IntelHexProvider::iHEX_DRMODE_16bit);
    }

    bool r = p.saveToHexFile(data ,fileName);

    if(r)
    {
        QFileInfo ii(fileName);
        tw->setTabText(tw->currentIndex(),ii.fileName());
        static_cast <HexEditorWidget *> (tw->currentWidget())->saved();
        on_dataSaved();
    }
    return r;
}

bool HexFilesMenager::loadFromMot(QString fileName)
{
    MotorolaSRecordProvider p;

    QFileInfo fi (fileName);

    QByteArray d;

    p.loadFromMotFile(fileName,d);

    this->on_addPage(d,fi.fileName());
    static_cast <HexEditorWidget *> (tw->currentWidget())->saved();
    on_dataSaved();
    return true;
}

bool HexFilesMenager::saveToMot(QString fileName)
{
    return false;
}

bool HexFilesMenager::saveToBinary(QString fileName)
{
    if(fileName.indexOf('.')==-1)fileName+=".bin";

    QFile f (fileName);

    f.open(QFile::WriteOnly);
    if(!f.isOpen())
    {

        return false;
    }
    f.write(this->getCurrentPage());

    f.close();

    QFileInfo ii(f);

    tw->setTabText(tw->currentIndex(),ii.fileName());

    static_cast <HexEditorWidget *> (tw->currentWidget())->saved();
    on_dataSaved();

    return true;

}

bool HexFilesMenager::loadFromBinary(QString fileName)
{

    QFileInfo fi (fileName);
    if (fileName.endsWith(".hex"))
    {
        if(QMessageBox::No == QMessageBox::question(dynamic_cast<QWidget*> (this->parent()),tr("Binary file loading"),tr("Hex file will be load as binary. Continue?"),QMessageBox::Yes,QMessageBox::No))
            return false;
    }


    HexEditorWidget * w = new HexEditorWidget(dynamic_cast<QWidget*> (this->parent()));
    w->setLineText(infoline);

    w->loadFromFile(fileName);


    tw->addTab(w,fi.fileName());
    tw->setCurrentIndex(tw->count()-1);
    static_cast <HexEditorWidget *> (tw->currentWidget())->saved();
    on_dataSaved();
    return true;
}


void HexFilesMenager::lockEditor()
{
    tw->setEnabled(false);

    int i = 0;
    while( i < tw->count())
    {
        static_cast <HexEditorWidget *> (tw->widget(i))->setEnabled(false);
        i++;
    }
}

void HexFilesMenager::unLockEditor()
{
    tw->setEnabled(true);

    int i = 0;
    while( i < tw->count())
    {
        static_cast <HexEditorWidget *> (tw->widget(i))->setEnabled(true);
        i++;
    }
}

bool HexFilesMenager::currentTabCloseRequested()
{
    deleteCurrentPage();
    return true;
}

bool HexFilesMenager::tabCloseRequested(int i)
{
    if(tw->count()>i)
    {

    tw->setCurrentIndex(i);

    if(!static_cast <HexEditorWidget *> (tw->currentWidget())->isChanged()) tw->removeTab(i);
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Close tab"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setInformativeText(tr("File not saved. Close tab and loose all changes?"));
        msgBox.setStandardButtons( QMessageBox::No | QMessageBox::Yes );
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if(ret == QMessageBox::No)
        {
            return false;
        }
        else
        {
            tw->removeTab(i);
        }
    }

    }
    return true;
}

bool HexFilesMenager::allTabCloseRequested()
{

    int i = 0;
    bool s = false;
    while( i < tw->count())
    {
        s = s | static_cast <HexEditorWidget *> (tw->widget(i))->isChanged();
        i++;
    }

    if(s)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Close all tabs"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setInformativeText(tr("Some files not saved. Close anyware?"));
        msgBox.setStandardButtons( QMessageBox::No | QMessageBox::Yes );
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if(ret == QMessageBox::No)
        {
            return false;
        }

    }

    while(tw->count()>0)
    {
        tw->removeTab(tw->count()-1);
    }
    return true;
}

int HexFilesMenager::pageCount()
{
    return tw->count();
}

QString HexFilesMenager::currentPageName()
{
    if(tw->count()>0)
    {
        return tw->tabText(tw->currentIndex());
    }
    return "";
}

void HexFilesMenager::on_dataChanged()
{
    if(tw->count()>0)
    {
        QString label = tw->tabText(tw->currentIndex());
        if (label[label.length()-1]!='*')label = label + "*";
        tw->setTabText(tw->currentIndex(),label);
    }
}

void HexFilesMenager::on_dataSaved()
{
    if(tw->count()>0)
    {
        QString label = tw->tabText(tw->currentIndex());
        if (label[label.length()-1]=='*')label = label.left(label.length()-1);
        tw->setTabText(tw->currentIndex(),label);

        //static_cast <HexEditorWidget *> (tw->currentWidget())->clearMask();
    }
}
