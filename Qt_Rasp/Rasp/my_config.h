#ifndef MY_CONFIG_H
#define MY_CONFIG_H

#include <QObject>
#include <QtDebug>
#include <QPoint>
#include <QtSerialPort/QSerialPort>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <QPoint>
#include <QList>

enum sensor_t
{
    temperature = 0,
    humidity,
    Sun_intensity,
    rain,
    speed,
    compass
};

class My_config : public QObject
{
    Q_OBJECT
public:
    explicit My_config(QObject *parent = nullptr);

public slots:
    void send_data(QString data);
    void connet();
    void stop_connet();
    bool is_a_number(QString number);
private slots:
    void serialport_read();
signals:
    void increaseOne(QString ms);
};

#endif // MY_CONFIG_H
