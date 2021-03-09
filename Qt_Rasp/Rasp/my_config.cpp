#include "my_config.h"

extern QSerialPort * serialPort;
QString buffer;
QString serial_input;
uint16_t value[6];
My_config::My_config(QObject *parent) : QObject(parent)
{

}
void My_config::send_data(QString data)
{
    if(serialPort->isOpen() == true)
    {
        qDebug()<<data;
        serialPort->write(data.toStdString().data());
    }
    else
    {
        qDebug()<<"Chua mo cong COM";
    }
}

void My_config::connet()
{
    serialPort->open((QIODevice::ReadWrite));
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(serialport_read()));
}

void My_config::stop_connet()
{
    serialPort->close();
}

bool My_config::is_a_number(QString number)
{
    for(int i = 0;i < number.size();i++)
    {
        if(number[i].isLetter()) return false;
    }
    return true;
}

void My_config::serialport_read()
{
    serial_input = serialPort->read(1024);
    serialPort->clear();
    if( serial_input != "W")
    {
        buffer += serial_input;
    }
    else
    {
        qDebug() << buffer;
        emit increaseOne(buffer);
        buffer = "";
    }
    serial_input = "";

}
