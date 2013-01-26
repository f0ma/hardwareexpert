#include "importexportmenager.h"

ImportExportMenager::ImportExportMenager(QObject *parent) :
    QObject(parent)
{
    importFormats["hex"] = "Intel Hex [generic] (*.hex)";
    importFormats["mot"] = "Motorola S-Record (*.mot)";
    importFormats["uoc"] = "UOC firmware (*.hex)";
    importFormats["stosd"] = "ST OSD File [os0 and os1 file] (*.os*)";


    exportFormats["hex"] = "Intel Hex [generic] (*.hex)";
//    exportFormats["mot"] = "Motorola S-Record (*.mot)";
    exportFormats["uoc"] = "UOC firmware (*.hex)";
    exportFormats["stosd"] = "ST OSD File [os0 and os1 file] (*.os*)";
}

void ImportExportMenager::setHexFilesMenager(HexFilesMenager * m)
{
     this->fm = m;
}

QString ImportExportMenager::getFormatImportList()
{
    return QStringList(importFormats.values()).join(";;");
}

QString ImportExportMenager::getFormatExportList()
{
    return QStringList(exportFormats.values()).join(";;");
}

bool ImportExportMenager::importFile (QString filename,QString format)
{
    qDebug() << filename << format;

    if(importFormats.value("hex")==format)
    {
        return fm->loadFromHex(filename);
    }

    if(importFormats.value("mot")==format)
    {
        return fm->loadFromMot(filename);
    }

    if(importFormats.value("uoc")==format)
    {
        return fm->loadFromHex(filename);
    }

    if(importFormats.value("stosd")==format)
    {
        StOSDFile f;
        QByteArray s = f.importFile(filename);
        if (s.length() == 0) return false;
        fm->addPage(s,"OSD");
         return true;
    }

    return false;
}

bool ImportExportMenager::exportFile (QString filename,QString format)
{
    qDebug() << filename << format;
    bool result = false;

    do{

    if(exportFormats.value("hex")==format)
    {
        result = fm->saveToHex(filename);
        break;
    }

    if(exportFormats.value("mot")==format)
    {
        result = fm->saveToMot(filename);
        break;
    }

    if(exportFormats.value("uoc")==format)
    {
        if(filename.indexOf('.')==-1)filename+=".hex";

        QByteArray data = fm->getCurrentPage();

        IntelHexProvider p;

        if (data.size()!=0x61000)
        {
            result = false;
            break;
        }


        p.setAddressing(IntelHexProvider::iHEX_AMODE_32bit);
        p.setDataRecordLength(IntelHexProvider::iHEX_DRMODE_32bit);

        MemoryMapRecord m1,m2,m3,m4;



        m1.startAddress = 0;
        m1.length = 0x10000; //64

        for(int i =0x10000 ; i<0x20000; i++)
        {
            if(data[i]!='\xFF'  && data[i]!= '\x00')
            {
                m1.length = 0x20000;
                break;
            }
        }

        for(int i =0x20000 ; i<0x30000 && m1.length == 0x20000; i++)
        {
            if(data[i]!='\xFF'  && data[i]!= '\x00')
            {
                m1.length = 0x30000;
                break;
            }
        }


        for(int i =0x30000 ; i<0x40000 && m1.length == 0x30000; i++)
        {
            if(data[i]!='\xFF'  && data[i]!= '\x00')
            {
                m1.length = 0x40000;
                break;
            }
        }


        if (m1.length == 0x10000 )ci->print(tr("PROG size detected: 64K\n"));
        if (m1.length == 0x20000 )ci->print(tr("PROG size detected: 128K\n"));
        if (m1.length == 0x30000 )ci->print(tr("PROG size detected: 192K\n"));
        if (m1.length == 0x40000 )ci->print(tr("PROG size detected: 256K\n"));



        m2.startAddress = 0x40000;
        m2.length= 0x2400;
        m3.startAddress = 0x50000;
        m3.length = 0x2400;
        m4.startAddress = 0x60000;
        m4.length = 0x1000;
        p.memoryAreas << m1 << m2 << m3 << m4;

        result = p.saveToHexFile(data ,filename);
        break;

    }

    if(exportFormats.value("stosd")==format)
    {
        if(filename.indexOf('.')==-1)filename+=".os0";
        StOSDFile f;
        QByteArray data = fm->getCurrentPage();
        ci->print(tr("Files os0 and os1 created.\n"));
        result = f.exportFile(filename,data);
    }

    }while(false);

    if(result)
    {
        ci->print(tr("File: ")+filename+tr(" export succsessful.\n"));
        fm->on_dataSaved();
    }

    return result;
}

void ImportExportMenager::setConsoleInterfaceMenager(ConsoleInterface * m)
{
    ci = m;
}
