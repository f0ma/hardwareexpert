#ifndef CODEFILE_H
#define CODEFILE_H

#include <QObject>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QList>
#include <QDir>
#include <QStringList>
#include <QDateTime>

class CodeFile : public QObject
{
    Q_OBJECT

    QPlainTextEdit * codeEditor;
    QTreeWidget * treeWidget;
    bool currentCodeChanged;
    bool onlyLoad;
    QString codeFileName;

    QStringList sourceDirectoryList;
    QStringList codeCategory;

    struct CodeFileStruct
    {
        QString category;
        QString name;
        QString desc;
        QString fileName;
        QString filePath;
    };

    QList<CodeFileStruct> codeFiles;

    void updateTree();

public:
    explicit CodeFile(QObject *parent = 0);
    void setCodeEditor(QPlainTextEdit * ple);
    void setSelectTreeWidget(QTreeWidget * tw);
    QString currentCodePath();

signals:
    void setFilenameIndicator(QString text);

public slots:
    void newFile();
    bool openFromFile();
    void saveToFile();
    void saveToFileAs();
    bool saveUnsaved();
    void updateLibrary(QStringList paths);

    bool codeChanged();

private slots:

    void treeWidgetItemDoubleClicked ( QTreeWidgetItem * item, int column );
    void planeEdittextChanged ();

};

#endif // CODEFILE_H
