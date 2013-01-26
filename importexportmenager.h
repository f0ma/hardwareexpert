#ifndef IMPORTEXPORTMENAGER_H
#define IMPORTEXPORTMENAGER_H

#include <QObject>
#include <QStringList>
#include "hexfilesmenager.h"
#include "consoleinterface.h"
#include "stOsdFile/stOsdFile.h"

class ImportExportMenager : public QObject
{
    Q_OBJECT
    QMap<QString,QString> importFormats;
    QMap<QString,QString> exportFormats;
    HexFilesMenager * fm;
    ConsoleInterface * ci;
public:
    explicit ImportExportMenager(QObject *parent = 0);
    QString getFormatImportList();
    QString getFormatExportList();
    bool importFile (QString filename,QString format);
    bool exportFile (QString filename,QString format);
    void setHexFilesMenager(HexFilesMenager * m);
    void setConsoleInterfaceMenager(ConsoleInterface * m);
signals:
    
public slots:
    
};

#endif // IMPORTEXPORTMENAGER_H
