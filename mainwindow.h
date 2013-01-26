#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTextCodec>
#include <QMap>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include <QPixmap>
#include <QBitmap>

#include "planetextconsoleinterface.h"
#include "slangwrapper.h"
#include "hexeditorwidget.h"
#include "tabpagerinterface.h"

#include "optionsdialog.h"

#include "codefile.h"
#include "codeeditor/codeeditor.h"

#include "helpwindow.h"
#include "driverdialog.h"
#include "optionsdialog.h"
#include "compairdialog.h"
#include "hexfilesmenager.h"
#include "importexportmenager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void runCodeOnLoad();
    ConsoleInterface * getConsole();

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    Ui::MainWindow *ui;
    SLangWrapper * itptr;
    QMap<int,HexEditorWidget*> hts;

    CodeFile * codeFileManager;

    ConsoleInterface * ci;
    CodeEditor * ce;

    HelpWindow * helpWindow;
    DriverDialog * driverWindow;
    OptionsDialog * optionWindow;

    QStringList waitImgs;
    QTimer * waitTimer;

    HexFilesMenager * hfm;
    ImportExportMenager * imp;


private slots:
    void on_actionNew_hex_triggered();
    void on_actionDiff_triggered();
    void on_actionSearch_triggered();
    void on_actionSearch_String_triggered();
    void on_actionDrivers_triggered();
    void on_actionOptions_triggered();
//    void on_actionSave_Hex_file_triggered();
//    void on_actionOpen_Hex_file_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void on_pbSaveLog_clicked();
    void on_pbClearLog_clicked();

    void on_pbExecute_clicked();
    void on_actionSave_hex_triggered();
    void on_actionSave_code_triggered();
    void on_actionSave_code_as_triggered();
    void on_actionNew_code_triggered();
    void on_actionOpen_Code_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_Hex_triggered();
    void on_stopButton_clicked();
    void on_runButton_clicked();

    void on_actionClose_all_binary_triggered();

    void on_actionFind_all_text_string_16_bit_triggered();

    void on_actionImportFile_triggered();

    void on_actionExportFile_triggered();

public slots:
    void onFilenameIndicatorChanged(QString s);
    void onScriptRunning();
    void onScriptStoped();
    void questUser(QString title,QString txt, int * result);
    void scriptChanged();
    void onProgressTimerTick();
    void callOptionDialog (QVector<ConsoleInterface::optionDialogEntry> * options,bool * ret);

    signals:
    void hexPageCountChange();
};

#endif // MAINWINDOW_H
