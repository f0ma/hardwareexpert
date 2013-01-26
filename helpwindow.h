#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QSplitter>
#include <QTextBrowser>
#include <QLayout>
#include <QPushButton>
#include <QFile>

namespace Ui {
    class HelpWindow;
}

class HelpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();


private:
    Ui::HelpWindow *ui;
    QTextBrowser * textBrowser;

public slots:
    void openIndex ();
};

#endif // HELPWINDOW_H
