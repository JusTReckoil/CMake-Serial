#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QThread>

class SerialThread : public QObject
{
    Q_OBJECT
public:
    bool connected = false;
    bool b;
    explicit SerialThread(QObject *parent = 0);

public slots:
    void serialConnect(QString port);
    void serialSend(unsigned long, unsigned long, unsigned long);
//    void serialClose();

private:
    int attenuation = 0;
    int timeout = 200;
    int adc_samples = 20;
    int intermid_freq = 10700000;
    int command_att = 10000 + ((attenuation) * 100);
    int command_number = 0;
    const int temp = 1000000;
    unsigned long step_to_send;
    unsigned long stop_to_send;
    unsigned long start_to_send;
    void serialRecive(unsigned long, unsigned long, unsigned long);
    float decode_binary(uint8_t firstbyte, uint8_t secondbyte, int &index);
    QSerialPort *serialPort;
signals:
    void finished();
    void sendMessage();
    void getFreq(QVector<double>);
    void getAmp(QVector<double>);
    void plot();
};

#endif // SERIALTHREAD_H
