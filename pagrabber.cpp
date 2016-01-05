#include "pagrabber.h"
#include "ui_pagrabber.h"

PAGrabber::PAGrabber(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PAGrabber)
{
    ui->setupUi(this);

    proc = new QProcess();
    proc->setWorkingDirectory(".");
    proc->start("./pactl subscribe");

    proc2 = new QProcess();
    proc2->setWorkingDirectory("/tmp");
    proc2->connect(proc2, SIGNAL( readyReadStandardOutput() ), this, SLOT( process2Output() ) );
    proc2->connect(proc2, SIGNAL( readyReadStandardError() ), this, SLOT( process2Output() ) );
    proc2->connect(proc2, SIGNAL(started()), this, SLOT(proc2Started()));
    proc2->connect(proc2, SIGNAL(finished(int)), this, SLOT(proc2Finished(int)));


    proc->connect(proc, SIGNAL( readyReadStandardOutput() ), this, SLOT( processOutput() ) );
    proc->connect(proc, SIGNAL( readyReadStandardError() ), this, SLOT( processOutput() ) );
    proc->connect(proc, SIGNAL(started()), this, SLOT(procStarted()));
    proc->connect(proc, SIGNAL(finished(int)), this, SLOT(procFinished(int)));
}

PAGrabber::~PAGrabber()
{
    delete ui;
}

void PAGrabber::proc2Started()
{
    ui->te->append("Proc2 started. PID "+QString::number(proc2->pid())+"\r\n");
}

void PAGrabber::proc2Finished(int exitStatus)
{
    ui->te->append("Proc2 finished. Code "+QString::number(exitStatus)+"\r\n");
}

void PAGrabber::procStarted()
{
    ui->te->append("Proc started. PID "+QString::number(proc->pid())+"\r\n");
}

void PAGrabber::procFinished(int exitStatus)
{
    ui->te->append("Proc finished. Code "+QString::number(exitStatus)+"\r\n");
    QTest::qWait(1000);
    proc->start("./pactl subscribe");
}

/*
[smooker@smoolap ~/src/pagrabber/PAGrabber]$ cat /tmp/asdf.log
Event 'new' on sink-input #38
Event 'change' on sink-input #38
Event 'change' on sink #0
Event 'new' on source-output #41
Event 'change' on sink-input #38
Event 'change' on sink #0
Event 'remove' on source-output #41
Event 'remove' on sink-input #3

parec -d steam.monitor | oggenc -b 192 -o /tmp/`date +%s`.ogg --raw -
paplay --raw --format=s16ne $cfile

bez ogg, che gi kastri neshto
 */

void PAGrabber::process2Output()
{
    QList<QByteArray> bal1;
    QList<QByteArray> bal2;
    bal1 = proc2->readAllStandardOutput().split('\n');
    bal2 = proc2->readAllStandardError().split('\n');

    foreach(QByteArray ba, bal1) {
        ui->te->append("OUT2:"+QString(ba));
    }

    foreach(QByteArray ba, bal2) {
         ui->te->append("ERR2:"+QString(ba));
    }
}

void PAGrabber::processOutput()
{
    QList<QByteArray> bal1;
    QList<QByteArray> bal2;
    bal1 = proc->readAllStandardOutput().split('\n');
    bal2 = proc->readAllStandardError().split('\n');

    foreach(QByteArray ba, bal1) {
         QString s1=QString(ba);
         ui->te->append("OUT:"+s1);

         QRegularExpression re("Event 'new' on sink-input #(\\d+)");
         QRegularExpressionMatch match = re.match(s1);
         if (match.hasMatch() && number.isEmpty()) {
             number = match.captured(1);
             ui->te->append("### START: #"+number);
             proc2->start("/usr/bin/parec", QStringList() << "-d" << "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor" << "/tmp/test.raw");
         }

         QRegularExpression re1("Event 'remove' on sink-input #"+number);
         QRegularExpressionMatch match1 = re1.match(s1);
         if (match1.hasMatch()) {
             ui->te->append("### END: #"+number);
             number = "";
             proc2->terminate();
             system("mv /tmp/test.raw /tmp/`date +%s`.raw");
         }
    }

    foreach(QByteArray ba, bal2) {
         ui->te->append("ERR:"+QString(ba));
    }
}

void PAGrabber::on_actionQuit_triggered()
{
    qApp->quit();
}

void PAGrabber::on_actionAbout_triggered()
{
    QMessageBox::information(0, "About", "Pulseaudio grabber\nwritten by smooker 2015\natvservice@mail.bg\nInspired by www.naturalreaders.com\nDonations are welcome :)", QMessageBox::Close, QMessageBox::Close);
}
