#ifndef HEXEDITORWIDGET_H
#define HEXEDITORWIDGET_H

#include <QWidget>
#include <QBitArray>
#include <QScrollBar>
#include <QLayout>
#include <QFile>
#include <QPainter>
#include <QKeyEvent>

#include "qhexedit/qhexedit.h"

class HexEditorWidget : public  QHexEdit
{

    Q_OBJECT

    QLabel * lbl;
    bool changed;

public:
    explicit HexEditorWidget(QWidget *parent = 0);
    bool loadFromFile(QString filename);
    bool loadFromHexFile(QString filename);
    bool saveToFile(QString filename);
    void setLineText(QLabel * label);

    bool isChanged();
    void saved();

public slots:
    void on_dataChanged();

private slots:

    void updateTextBar(int addr);

};

#endif // HEXEDITORWIDGET_H
