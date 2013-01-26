#ifndef PLANETEXTCONSOLEINTERFACE_H
#define PLANETEXTCONSOLEINTERFACE_H

#include <QPlainTextEdit>
#include <QProgressBar>
#include <QMessageBox>
#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QVector>
#include "consoleinterface.h"

class PlaneTextConsoleInterface : public QObject , public ConsoleInterface
{
    Q_OBJECT;

    QPlainTextEdit * te;
    QProgressBar * pb;
    QVector <QString> * cons;
    QMutex cons_mutex;

    QTimer * upd;
    bool change;

    int progVal;
public:
    PlaneTextConsoleInterface();
    void setPlaneTextEdit(QPlainTextEdit * t);
    void setProgressBar(QProgressBar * p);
    void print(QString s);
    void clear();
    void rmline();
    void setProgressValue(int s);
    void setProgressMax(int s);
    void setProgressWait();
    void clearProgress();
    int quest(QString title,QString text);
    bool optionDialog(QVector<optionDialogEntry> * options);
signals:
    void appendToConsole(QString s);
    void setConsole(QString s);
    void clearConsole();

    void setProgbarValue(int v);
    void setProgbarMax(int m);

    void quest(QString title,QString text,int * result);
    void callOptionDialog (QVector<ConsoleInterface::optionDialogEntry> * options,bool * ret);

public slots:
    void updateTimer();


};

#endif // PLANETEXTCONSOLEINTERFACE_H
