#ifndef INTELHEXPROVIDER_H
#define INTELHEXPROVIDER_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QVector>

struct MemoryMapRecord
{
    int startAddress;
    int length;
};

class IntelHexProvider
{
public:

    enum IntelHexAddressing {iHEX_AMODE_8bit,iHEX_AMODE_16bit,iHEX_AMODE_32bit};
    enum IntelHexDataRecord {iHEX_DRMODE_8bit,iHEX_DRMODE_16bit,iHEX_DRMODE_32bit};
    QVector <MemoryMapRecord> memoryAreas;

private:

    unsigned int dataRecordLength;
    IntelHexAddressing addressingMode;
    unsigned int currentAddress;

public:

    IntelHexProvider();

    bool loadFromHexFile(QString name, QByteArray & data);

    void setAddressing (IntelHexAddressing mode);
    void setDataRecordLength (IntelHexDataRecord mode);

    bool saveToHexFile(QByteArray data,QString name);
//    bool saveToHexFile(QByteArray data,QString name,QVector<MemoryMapRecord>);

    ~IntelHexProvider();
};

#endif // INTELHEXPROVIDER_H
