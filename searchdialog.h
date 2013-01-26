#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "hexeditorwidget.h"

namespace Ui {
    class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    void setHexEditor(HexEditorWidget * h);
    void setText(QString s);
    ~SearchDialog();

private:
    Ui::SearchDialog *ui;
    HexEditorWidget * editor;
    QValidator *hexValidator;
    QStringList results;
    QStringListModel model_results;
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pbSearch_clicked();
    void on_rbHexMode_clicked();
    void on_rbAsciiMode_clicked();
    void on_lvResults_clicked(const QModelIndex &index);
};

#endif // SEARCHDIALOG_H
