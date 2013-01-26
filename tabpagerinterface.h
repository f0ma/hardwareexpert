#ifndef TABPAGERINTERFACE_H
#define TABPAGERINTERFACE_H

#include "memorypagerinterface.h"

#include "hexeditorwidget.h"

#include <QTabWidget>

class TabPagerInterface : public QObject,public MemoryPagerInterface
{
Q_OBJECT;

public:
    TabPagerInterface();

    void addPage(QByteArray arr,QString text);
    void setCurrentPage(QByteArray * arr);
    QByteArray getCurrentPage();
    void deleteCurrentPage();

    void setTabWidget(QTabWidget * atw);
    void setParentWidget(QWidget * apwt);

private:
    QTabWidget * tw;
    QWidget * pwt;
signals:
    void addPageSignal(QByteArray arr,QString text);
    void overwritePageSignal(QByteArray * arr);
};

#endif // TABPAGERINTERFACE_H
