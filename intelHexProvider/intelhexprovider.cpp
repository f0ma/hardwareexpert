#include "intelhexprovider.h"

IntelHexProvider::IntelHexProvider()
{
    addressingMode=iHEX_AMODE_32bit;
    currentAddress=0;
    dataRecordLength=16;
}

//enum IntelHexAddressing {iHEX_AMODE_8bit,iHEX_AMODE_16bit,iHEX_AMODE_32bit};
//enum IntelHexDataRecord {iHEX_DRMODE_8bit,iHEX_DRMODE_16bit,iHEX_DRMODE_32bit};


bool IntelHexProvider::loadFromHexFile(QString name, QByteArray & data)
{
    currentAddress=0;
    unsigned int lastAddress=0;
    bool useAutoCut=false;

    if(data.size()==0)
    {
        data.fill(0x00,0x10000000);
        useAutoCut=true;
    }

    QFile hexFile(name);
    if(!hexFile.open(QIODevice::ReadOnly))return false;
    while(!hexFile.atEnd())
    {
        QByteArray srecord = hexFile.readLine();
        if(srecord[srecord.size()-1]=='\n') srecord=srecord.left(srecord.size()-1);
        if(srecord[srecord.size()-1]=='\r') srecord=srecord.left(srecord.size()-1);
        if(srecord[0]!=':')continue; // not record

        srecord=srecord.right(srecord.length()-1);
        QByteArray record = QByteArray::fromHex(srecord);

        if(record.size()!=(record[0]+5)) return false; // invalid size

        char crc=0;

        for(int i=0;i<record.size()-1;i++)
        {
            crc+=record[i];
        }

        crc=0x100-crc;

        if(crc!=record[record.size()-1]) return false; // invalid crc

        if(record[3]=='\x0') //data record
        {
            unsigned int dataAddress =0;
            unsigned char r1 = record[1];
            unsigned char r2 = record[2];

            dataAddress = r1*0x100+r2;

//            if(dataAddress+currentAddress>=data.size())return false;

            for(int i=0;i<record[0];i++)
            {
                unsigned int dataAdr=dataAddress+currentAddress+i;
                data[dataAdr]=record[i+4];
                if(lastAddress<dataAdr)
                    lastAddress=dataAdr;
            }

            continue;
        }

        if(record[3]=='\x1') //EOF
        {
            break;
        }

        if(record[3]=='\x2') //20 bit EA
        {
            currentAddress=0;
            currentAddress=record[4]*0x100+record[5];
            currentAddress=currentAddress<<4;
            continue;
        }

        if(record[3]=='\x4') //32 bit EA
        {
            currentAddress=0;
            currentAddress=record[4]*0x100+record[5];
            currentAddress=currentAddress<<16;
            continue;
        }
    }

    if(useAutoCut)
    {
        data = data.left(lastAddress+1);
    }
    return true;
}

void IntelHexProvider::setAddressing (IntelHexAddressing mode)
{
    addressingMode=mode;
}

void IntelHexProvider::setDataRecordLength (IntelHexDataRecord mode)
{
    switch(mode)
    {
    case iHEX_DRMODE_8bit:
        dataRecordLength=8;
        break;
    case iHEX_DRMODE_16bit:
        dataRecordLength=16;
        break;
    case iHEX_DRMODE_32bit:
        dataRecordLength=32;
        break;
    }
}

bool IntelHexProvider::saveToHexFile(QByteArray data,QString name)
{
    if(addressingMode==iHEX_AMODE_8bit && data.size() >0xFFFF)return false;
    if(addressingMode==iHEX_AMODE_16bit && data.size() >0xFFFFF)return false;
    if(addressingMode==iHEX_AMODE_32bit && data.size() >0x0FFFFFFF)return false;

    QFile hexFile(name);
    if(!hexFile.open(QIODevice::WriteOnly))return false;


    if (memoryAreas.size()==0)
    {
        MemoryMapRecord m;
        m.startAddress = 0;
        m.length = data.size();
        memoryAreas.append(m);
    }

    MemoryMapRecord cm;

    foreach(cm,memoryAreas)
    {

    for(int adr=cm.startAddress;adr<cm.startAddress+cm.length;adr+=dataRecordLength)
    {
        if(adr % 0x10000 == 0 && addressingMode!=iHEX_AMODE_8bit)
        {
        QByteArray arecord;
            if(addressingMode==iHEX_AMODE_16bit)
            {
                arecord.append('\x02');
                arecord.append('\x00');
                arecord.append('\x00');
                arecord.append('\x02');
                arecord.append((((adr>>16)<<12))>>8);
                arecord.append((((adr>>16)<<12)));

            }
            if(addressingMode==iHEX_AMODE_32bit)
            {
                arecord.append('\x02');
                arecord.append('\x00');
                arecord.append('\x00');
                arecord.append('\x04');
                arecord.append((adr>>16)>>8);
                arecord.append((adr>>16));
            }

            unsigned char crc=0;

            for(int i=0;i<arecord.size();i++)
            {
                crc+=arecord[i];
            }

            crc=0x100-crc;

            arecord.append(crc);

            hexFile.write(arecord.toHex().toUpper().append("\x0D\x0A").prepend(':'));
        }

        QByteArray srecord;

        srecord[0]=dataRecordLength;
        srecord[1]=adr>>8;
        srecord[2]=adr;
        srecord[3]=0;
        srecord.append(data.mid(adr,dataRecordLength));

        if((unsigned)srecord.size()<4+dataRecordLength) srecord[0]=srecord.size()-4;


        unsigned char crc=0;

        for(int i=0;i<srecord.size();i++)
        {
            crc+=srecord[i];
        }

        crc=0x100-crc;

        srecord.append(crc);

        hexFile.write(srecord.toHex().toUpper().append("\x0D\x0A").prepend(':'));
    }
    }

    hexFile.write(":00000001FF\x0D\x0A");

    hexFile.close();

    return true;
}

//bool IntelHexProvider::saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord>)
//{
//    Q_UNUSED(data);
//    Q_UNUSED(name);
//    return false;
//}

IntelHexProvider::~IntelHexProvider()
{
}

