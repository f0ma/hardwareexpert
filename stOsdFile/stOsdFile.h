#ifndef ST_OSD_FILE
#define ST_OSD_FILE

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QFile>

class StOSDFile
{
public:
    QByteArray importFile(QString filename);
    bool exportFile(QString filename,QByteArray rdata);
};



#endif
