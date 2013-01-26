#ifndef SLANGTHREAD_H
#define SLANGTHREAD_H

#include <QThread>
#include <QRegExp>
#include <slang.h>

class SLangThread : public QThread
{
    Q_OBJECT

    char * code;
    char * foo;

public:

    SLangThread ( QObject * parent = 0 );
    void setScript(QString s);
    void setFunction(QString s);

protected:
    void run();
};

#endif // SLANGTHREAD_H
