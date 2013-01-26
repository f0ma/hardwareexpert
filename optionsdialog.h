#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void sendPathList();

private:
    Ui::OptionsDialog *ui;

private slots:
    void on_cbSaveGuiState_stateChanged(int );
    void on_sbTrunkLen_valueChanged(int );
    void on_cbTrunkLog_stateChanged(int );
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_bpBrowse_clicked();
    void on_cbExecuteNow_stateChanged(int );
    void on_cbUseBackup_stateChanged(int );
signals:
    void updateScriptLibrary(QStringList l);
};

#endif // OPTIONSDIALOG_H
