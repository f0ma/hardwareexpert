#ifndef MEMORYPAGERINTERFACE_H
#define MEMORYPAGERINTERFACE_H

#include <QByteArray>

class MemoryPagerInterface
{
public:
    MemoryPagerInterface(){};

    virtual void addPage(QByteArray arr,QString text)=0;
    virtual void setCurrentPage(QByteArray * arr)=0;
    virtual QByteArray getCurrentPage()=0;
    virtual void deleteCurrentPage()=0;

    virtual ~MemoryPagerInterface(){};
};

#endif // MEMORYPAGERINTERFACE_H
