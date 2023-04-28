#include "serialthread.h"

SerialThread::SerialThread(QObject *parent) : QObject(parent)
{

}

void SerialThread::serialConnect(QString port)
{
    //Задаем параметры для общения по сериал порту
    serialPort = new QSerialPort(this);
    serialPort->setPortName(port);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(!serialPort->open(QIODevice::ReadWrite))
    {
        qDebug() << "Fail!";
        //Выставляем флаг, что не подключчены к порту
        connected = false;
        return;
    }
    else
    {
        //ui->connect->setText("Подключено");
        qDebug() << "Connected";
        //Выставляем флаг, что подключились
        connected = true;
    }
}

void SerialThread::serialSend(unsigned long start, unsigned long stop, unsigned long step)
{
    while(b)
    {
        start_to_send = (start * temp); // Стартовая частота для отправки в Гц
        stop_to_send = (stop * temp);   // Конечная частота для отправки в Гц
        step_to_send = (step * temp);   // Шаг для отправки в Гц
        //Сборка сообщения и преобразование в массив байтов
        QString msg = QString("scn20 %1 %2 %3 %4 %5 %6 %7 %8\r\n").arg(start_to_send).arg(stop_to_send).arg(step_to_send).arg(timeout).arg(adc_samples).arg(intermid_freq).arg(command_att).arg(command_number);
        QByteArray message = msg.toUtf8();
        QThread::msleep(50);    // Пауза перед отправкой
        if(serialPort->isOpen()){
            serialPort->write(message);
            //qDebug() << message;
        }
        serialRecive(start, stop, step);
        command_number++;
        if(command_number == 10)
            command_number = 0;
    }
    emit finished();
}

void SerialThread::serialRecive(unsigned long start, unsigned long stop, unsigned long step)
{
    QByteArray data;
    while(serialPort->waitForReadyRead(15))
    {
        data += serialPort->readAll();
    }

    //qDebug() << "Do: " << data.size() << "\n";
    data.remove(0, 24);
    data.truncate(data.size() - 18);
    //qDebug() << data.size();
    qDebug() << data;
    int index = data.size();
    QVector<double> amplitudeResultVec;
    if(data.size() == 1702)
    {
        for (int i = 0; i < data.size(); i += 2)
        {

            uint8_t firstbyte = static_cast<uint8_t>(data[i]);
            uint8_t secondbyte = static_cast<uint8_t>(data[i + 1]);
            float amplitude = decode_binary(firstbyte, secondbyte, index);
                amplitudeResultVec.append(amplitude);
         }
        QVector<double> frequncy;
        for (double i = start; i <= stop; i += step)
            frequncy.append(i);
        emit getFreq(frequncy);
        emit getAmp(amplitudeResultVec);
    }
}

float SerialThread::decode_binary(uint8_t firstbyte, uint8_t secondbyte, int &index)
{
    uint16_t val = (firstbyte << 8) | secondbyte;
    uint16_t data = val & 0b0000011111111111;
    index =  (val & 0b1111100000000000)>>11;
    float amplitude = (10.0*80.0 - data)/10.0;
    return amplitude;
}
