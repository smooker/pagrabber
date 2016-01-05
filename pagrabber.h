#ifndef PAGRABBER_H
#define PAGRABBER_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QTest>
#include <QMessageBox>

namespace Ui {
class PAGrabber;
}

class PAGrabber : public QMainWindow
{
    Q_OBJECT

public:
    explicit PAGrabber(QWidget *parent = 0);
    ~PAGrabber();

private:
    Ui::PAGrabber *ui;
    QProcess *proc;
    QProcess *proc2;
    QString number;

private slots:
    void processOutput();
    void procStarted();
    void procFinished(int exitStatus);

    void process2Output();
    void proc2Started();
    void proc2Finished(int exitStatus);
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
};

#endif // PAGRABBER_H
