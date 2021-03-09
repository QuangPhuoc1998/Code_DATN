#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <radialbar.h>
#include "qserialport.h"
#include <QSerialPortInfo>
#include <QDebug>
#include "my_config.h"
#include <QQmlEngine>
#include <QQmlContext>

QSerialPort * serialPort;
void Port_Init();
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<RadialBar>("CustomControls", 1, 0, "RadialBar");

    QQmlApplicationEngine engine;

    QQmlContext* context = engine.rootContext();
    My_config dt;
    context->setContextProperty("My_config", &dt);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    Port_Init();

    return app.exec();
}
void Port_Init()
{
    serialPort = new QSerialPort();
    serialPort->setPortName("COM3");
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
}

