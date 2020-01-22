#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "chatcontroller.h"
#include "message.h"

#include "messagesmodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);    

    qmlRegisterType<MessagesModel>("Messages",1,0,"MessagesModel");
    //qmlRegisterType<ChatController>("MyChat",1,0,"ChatController");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
