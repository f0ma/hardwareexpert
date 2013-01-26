#ifndef CONSOLEINTERFACE_H
#define CONSOLEINTERFACE_H

#include <QString>
#include <QStringList>

class ConsoleInterface
{
public:
    enum OPT_TYPE {OPT_LIST,OPT_CHK,OPT_STR,OPT_DEC,OPT_HEX};

    struct optionDialogEntry
    {
        QString name;
        QString text;
        OPT_TYPE type;
        int valueInt;
        QString valueStr;
        QStringList variants;
    };

    ConsoleInterface(){};
    virtual void print(QString s)=0;
    virtual void clear()=0;
    virtual void rmline()=0;
    virtual void setProgressValue(int s)=0;
    virtual void setProgressMax(int s)=0;
    virtual void setProgressWait()=0;
    virtual void clearProgress()=0;

    virtual int quest(QString title,QString text)=0;
    virtual bool optionDialog(QVector<optionDialogEntry> * options)=0;

    virtual ~ConsoleInterface(){};
};

#endif // CONSOLEINTERFACE_H
