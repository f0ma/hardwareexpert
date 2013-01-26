#ifndef COMPAIRDIALOG_H
#define COMPAIRDIALOG_H

#include <QDialog>
#include "hexeditorwidget.h"

namespace Ui {
    class CompairDialog;
}

class CompairDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompairDialog(QWidget *parent = 0);
    ~CompairDialog();
    void addEditor(HexEditorWidget * ed, QString name);

private:
    Ui::CompairDialog *ui;
    void closeEvent(QCloseEvent *event);
    HexEditorWidget * aEditor;
    HexEditorWidget * bEditor;
    QVector<HexEditorWidget *> edList;
    QVector<QString> edNames;
    bool cLock;
    QStringList result;
    QStringListModel resultModel;

    private slots:
    void on_pbCompare_clicked();
    void updateCombo();
    void updateComboA(int n);
    void updateComboB(int n);
    void on_lvResults_clicked(const QModelIndex &index);
};

#endif // COMPAIRDIALOG_H
