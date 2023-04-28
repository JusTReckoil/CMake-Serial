#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialthread.h"
#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow *ui;
    SerialThread *serial;
    QThread *thread;
    ~MainWindow();

public slots:
    void setFreq(QVector<double>);
    void setAmp(QVector<double>);

private slots:
    void on_refresh_clicked();
    void on_connect_clicked();
    void on_send_clicked();
    void on_disconnect_clicked();

signals:
    void startOperation(QString port);
    void sendMessage(unsigned long, unsigned long, unsigned long);
    void closePort();

private:
    QVector<double> amplitudeResultVec;
    QVector<double> frequncy;
};
#endif // MAINWINDOW_H
