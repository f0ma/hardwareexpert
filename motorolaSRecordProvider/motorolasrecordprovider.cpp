#include "motorolasrecordprovider.h"

MotorolaSRecordProvider::MotorolaSRecordProvider()
{
    addressingMode=mMOT_AMODE_24bit;
    dataRecordLength=16;
}


bool MotorolaSRecordProvider::loadFromMotFile(QString name, QByteArray & data)
{
	MotorolaSRecordHeader h;
    return loadFromMotFile(name,data,h);
}


bool MotorolaSRecordProvider::loadFromMotFile(QString name, QByteArray & data,MotorolaSRecordHeader & h)
{
    Q_UNUSED(h);
    int lastAddress = 0;
    unsigned int currentAddress;
    int bytesCount = 0;
    unsigned char crc;
    data.clear();


    QFile motFile(name);
    if(!motFile.open(QIODevice::ReadOnly))return false;
    while(!motFile.atEnd())
    {
        QByteArray srecord = motFile.readLine();
        if(srecord[srecord.size()-1]=='\n') srecord=srecord.left(srecord.size()-1);
        if(srecord[srecord.size()-1]=='\r') srecord=srecord.left(srecord.size()-1);

        srecord[0] = '0';

        QByteArray record;
        record=  QByteArray::fromHex(srecord);
        QByteArray ca;

        crc = 0;

        for(int i=0;i<record.size()-1;i++)
        {
            crc+=record[i];
        }

        crc=0x100-crc+1;

//        if(crc!=(unsigned char)record[record.size()-1]) return false; // invalid crc

        bytesCount = record[1];

        if((int)record[0]==0) // header record
        {
            // do nothing. no examples found
            continue;
        }

        if((int)record[0]==1) // 2 address bytes
        {
            ca = record.left(4).right(2);
            unsigned char a = ca.at(0);
            unsigned char b = ca.at(1);
            currentAddress = a*0x100+b;

            bytesCount=bytesCount-1-2;
        }

        if((int)record[0]==2) // 3 address bytes
        {
            ca = record.left(5).right(3);
            unsigned char a = ca.at(0);
            unsigned char b = ca.at(1);
            unsigned char c = ca.at(2);
            currentAddress = a*0x10000+b*0x100+c;
            bytesCount=bytesCount-1-3;
        }

        if((int)record[0]==3) // 4 address bytes
        {
            ca = record.left(6).right(4);
            unsigned char a = ca.at(0);
            unsigned char b = ca.at(1);
            unsigned char c = ca.at(2);
            unsigned char d = ca.at(3);
            currentAddress = a*0x1000000+b*0x10000+c*0x100+d;

            bytesCount=bytesCount-1-4;
        }

        //other type record not containing data

        if((int)record[0]==4 || (int)record[0]==5 || (int)record[0]==6 || (int)record[0]==7 || (int)record[0]==8 || (int)record[0]==9)
        {
            continue;
        }


        record = record.left(record.length()-1).right(bytesCount);


        if(currentAddress +record.length() > abs(data.length()))
        {
            QByteArray br;
            br.fill(0x00,currentAddress+record.length());

            data.append(br);
        }

        data.replace(currentAddress,record.length(),record);

        lastAddress = currentAddress + record.length();

}

    data = data.left(lastAddress);
return true;
}

void MotorolaSRecordProvider::setAddressing (MotorolaSRecordAddressing mode)
{
    addressingMode = mode;
}

bool MotorolaSRecordProvider::saveToMotFile(QByteArray data,QString name)
{
    MotorolaSRecordHeader h;
    return saveToMotFile(data,name,h);
}

bool MotorolaSRecordProvider::saveToMotFile(QByteArray data,QString name,MotorolaSRecordHeader h)
{
    Q_UNUSED(data);
    Q_UNUSED(name);
    Q_UNUSED(h);
    return false;
}

//bool MotorolaSRecordProvider::saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord> map)
//{
//    MotorolaSRecordHeader h;
//    return saveToHexFile(data,name,map,h);
//}

//bool MotorolaSRecordProvider::saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord> map,MotorolaSRecordHeader h)
//{
//    return false;
//}

MotorolaSRecordProvider::~MotorolaSRecordProvider()
{

}
