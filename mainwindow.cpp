#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
unsigned long start = 2400;
unsigned long stop = 2485;
double step = 0.1;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Просматриваем все подключенные порты  на момент открытия программы
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->cmbPort->addItem(serialPortInfo.portName());
        qDebug() << serialPortInfo.portName();
    }

    //Прячем кнопки, пока не подключились
    ui->disconnect->hide();
    ui->send->hide();

    //Создаем новый поток
    thread = new QThread(this);
    connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));

    //Создаем объект класса Сериал
    serial = new SerialThread();

    //Костыль, чтобы передавать вектор сигналом
    qRegisterMetaType<QVector<double> >("QVector<double>");

    //Коннекты, для работы с Сериал портом
    connect(this, SIGNAL(startOperation(QString)), serial, SLOT(serialConnect(QString)));
    connect(this, SIGNAL(sendMessage(unsigned long, unsigned long, unsigned long)), serial, SLOT(serialSend(unsigned long, unsigned long, unsigned long)));
    connect(this, SIGNAL(closePort()), serial, SLOT(serialClose()));
    connect(serial, SIGNAL(getFreq(QVector<double>)), this, SLOT(setFreq(QVector<double>)));
    connect(serial, SIGNAL(getAmp(QVector<double>)), this, SLOT(setAmp(QVector<double>)));
    connect(serial, SIGNAL(plot()), this, SLOT(plotGraph()));

    //Перемещаем Сериал в отдельный поток
    serial->moveToThread(thread);
}

MainWindow::~MainWindow()
{
    emit closePort();
    thread->exit();
    thread->wait();
    delete ui;
}

void MainWindow::on_refresh_clicked()
{
    ui->cmbPort->clear();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->cmbPort->addItem(serialPortInfo.portName());
        qDebug() << serialPortInfo.portName();
    }
}

void MainWindow::on_connect_clicked()
{
    QString port = ui->cmbPort->currentText();
    emit startOperation(port);
    if(serial->connected)
    {
        ui->disconnect->show();
        ui->send->show();
    }
}

void MainWindow::on_send_clicked()
{
    thread->start();
    serial->b = true;
    emit sendMessage(start, stop, step);
}

void MainWindow::on_disconnect_clicked()
{
    serial->b = false;
    emit closePort();
    thread->quit();
}

void MainWindow::setFreq(QVector<double> freq)
{
    frequncy = freq;
  //  qDebug() << frequncy;
}

void MainWindow::setAmp(QVector<double> ampl)
{
    amplitudeResultVec = ampl;
    qDebug() << amplitudeResultVec;
}




