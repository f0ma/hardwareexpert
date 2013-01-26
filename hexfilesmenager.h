#ifndef HEXFILESMENAGER_H
#define HEXFILESMENAGER_H

#include <QObject>
#include <QTabWidget>
#include <QFileInfo>
#include "hexeditorwidget.h"
#include "memorypagerinterface.h"
#include "intelHexProvider/intelhexprovider.h"
#include "motorolaSRecordProvider/motorolasrecordprovider.h"

class HexFilesMenager : public QObject,public MemoryPagerInterface
{
    Q_OBJECT
public:
    explicit HexFilesMenager(QObject *parent = 0);
    void addPage(QByteArray arr,QString text);
    void setCurrentPage(QByteArray * arr);
    QByteArray getCurrentPage();
    void deleteCurrentPage();



    bool loadFromBinary(QString fileName);
    bool saveToBinary(QString fileName);

    bool loadFromHex(QString fileName);
    bool saveToHex(QString fileName);

    bool loadFromMot(QString fileName);
    bool saveToMot(QString fileName);

    void setTabWidget(QTabWidget * atw);
    void setInfoLine(QLabel * il);

    int pageCount();
    QString currentPageName();

private:
    QTabWidget * tw;
    QLabel * infoline;

signals:
    void saveCurrentFile();
    void enableFilePrcessing(bool);

    void sig_addPage(QByteArray arr,QString text);
    void sig_setCurrentPage(QByteArray * arr);
    void sig_deleteCurrentPage();

public slots:
    void lockEditor();
    void unLockEditor();
    bool currentTabCloseRequested();
    bool tabCloseRequested(int i);
    bool allTabCloseRequested();

    void on_addPage(QByteArray arr,QString text);
    void on_setCurrentPage(QByteArray * arr);
    void on_deleteCurrentPage();

    void on_dataChanged();
    void on_dataSaved();

};

#endif // HEXFILESMENAGER_H
