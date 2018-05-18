#ifndef MQTTTHREAD_H
#define MQTTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QByteArray>

#include "pahomqtt/MQTTClient.h"

extern QByteArray imageData;

class MqttThread : public QThread
{
    Q_OBJECT
public:
    MqttThread();
    ~MqttThread();
    void messageArrived(MQTT::MessageData& md);
    void stop();
    bool isStopped() { return mStopped; }
    bool isConnected() { return mIsConnected; }
    void setImageData(QByteArray data) {
        imageData = data;
        qDebug() << imageData;
    }

protected:
    void run();

private:
    QMutex mutex;
    volatile bool mStopped;
    volatile bool mIsConnected;
    volatile long mArrivedcount;
    volatile int mRetry;

signals:
    void messageReceived(QString);
    void mqttDisconnected();

};

#endif // MQTTTHREAD_H
