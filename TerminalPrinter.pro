#-------------------------------------------------
#
# Project created by QtCreator 2018-04-19T16:30:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TerminalPrinter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    pahomqtt/linux.cpp \
    pahomqtt/MQTTConnectClient.c \
    pahomqtt/MQTTConnectServer.c \
    pahomqtt/MQTTDeserializePublish.c \
    pahomqtt/MQTTFormat.c \
    pahomqtt/MQTTPacket.c \
    pahomqtt/MQTTSerializePublish.c \
    pahomqtt/MQTTSubscribeClient.c \
    pahomqtt/MQTTSubscribeServer.c \
    pahomqtt/MQTTUnsubscribeClient.c \
    pahomqtt/MQTTUnsubscribeServer.c \
    mqttthread.cpp

HEADERS += \
        mainwindow.h \
    pahomqtt/FP.h \
    pahomqtt/linux.h \
    pahomqtt/MQTTClient.h \
    pahomqtt/MQTTConnect.h \
    pahomqtt/MQTTFormat.h \
    pahomqtt/MQTTLogging.h \
    pahomqtt/MQTTPacket.h \
    pahomqtt/MQTTPublish.h \
    pahomqtt/MQTTSubscribe.h \
    pahomqtt/MQTTUnsubscribe.h \
    pahomqtt/StackTrace.h \
    mqttthread.h

FORMS += \
        mainwindow.ui
