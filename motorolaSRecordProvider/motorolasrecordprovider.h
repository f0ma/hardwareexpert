#ifndef MOTOROLASRECORDPROVIDER_H
#define MOTOROLASRECORDPROVIDER_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QVector>

//struct MemoryMapRecord
//{
//    int startAddress;
//    int length;
//};

struct MotorolaSRecordHeader
{
QString mname;
QString description;
unsigned int ver;
unsigned int rev;
};

class MotorolaSRecordProvider
{
public:

    enum MotorolaSRecordAddressing {mMOT_AMODE_8bit,mMOT_AMODE_16bit,mMOT_AMODE_24bit,mMOT_AMODE_32bit};

private:

    unsigned int dataRecordLength;
    MotorolaSRecordAddressing addressingMode;

public:

    MotorolaSRecordProvider();

    bool loadFromMotFile(QString name, QByteArray & data);
    bool loadFromMotFile(QString name, QByteArray & data,MotorolaSRecordHeader & h);

    void setAddressing (MotorolaSRecordAddressing mode);
    
    bool saveToMotFile(QByteArray data,QString name);
    bool saveToMotFile(QByteArray data,QString name,MotorolaSRecordHeader h);
//    bool saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord> map);
//    bool saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord> map,MotorolaSRecordHeader h);

    ~MotorolaSRecordProvider();
};

#endif // MOTOROLASRECORDPROVIDER_H
