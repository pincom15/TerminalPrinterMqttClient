#include "mqttthread.h"
#include "pahomqtt/linux.cpp"
#include <QDateTime>


MqttThread::MqttThread()
{
    mStopped = false;
    mIsConnected = false;
    mArrivedcount = 0;
    mRetry = 0;
}

MqttThread::~MqttThread()
{
    wait();
}

void MqttThread::stop()
{
    QMutexLocker locker(&mutex);
    mStopped = true;
    mRetry = 0;
}

void MqttThread::messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;

    qDebug() << " ########## MQTT messageArrived ########################";
    qDebug() << " ########## MQTT Message arrived:" << ++mArrivedcount << ", qos:"  << message.qos << ", retained:" << message.retained << ", dup:" << message.dup << ", packetid:" << message.id;
    qDebug() << " ########## MQTT Payload [" << message.payloadlen << "] " << QByteArray((char*)message.payload, message.payloadlen);

    emit messageReceived(QString((char*)message.payload));
}

void MqttThread::run()
{
    {
        QMutexLocker locker(&mutex);
        mStopped = false;
        mIsConnected = false;
    }

    IPStack ipstack = IPStack();

    MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

    QByteArray mMqttHost = "10.211.55.5";
    const int port = 1883;

    // Exponential backoff
    if (mRetry > 0) {
        if (mRetry > 5) {
            mRetry = 5;
        }
        qsrand(time(NULL));
        unsigned long backoff = qrand() % (((2 << mRetry) - 1) * 4000000); // 0, 4, 12, 28, 60, 124 sec
        qDebug() << "Exp Backoff:" << backoff / 1000000 << " sec" << " Retry:" << mRetry;
        usleep(backoff);
    }

    qDebug() << " ########## MQTT Connecting to " << mMqttHost << ":" << port << ", Retry:" << mRetry;

    int rc = ipstack.connect(mMqttHost.data(), port);
    if (rc != 0) {
        mRetry++;
        ipstack.disconnect();
        qDebug() << " ########## MQTT rc from TCP connect is " << rc;

        emit mqttDisconnected();
        return;
    }

    qDebug() << " ########## MQTT connecting";
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    QByteArray clientId = QByteArray("PrinterTest");
    data.clientID.cstring = clientId.data();
    data.cleansession = 1;
    data.keepAliveInterval = 60;

    rc = client.connect(data);
    if (rc != 0) {
        mRetry++;
        qDebug() << " ########## MQTT rc from MQTT connect is " << rc;
        ipstack.disconnect();

        qDebug() << "emit mqttDisconnected()!! from MQTT Thread.";
        emit mqttDisconnected();
        return;
    }
    qDebug() << " ########## MQTT connected";

    QByteArray topic = "terminal01/status";
    rc = client.subscribe(topic.data(), MQTT::QOS1, this, &MqttThread::messageArrived);
    if (rc != 0) {
        mRetry++;
        qDebug() << " ########## MQTT rc from MQTT subscribe is " << rc;
        client.disconnect();
        ipstack.disconnect();

        qDebug() << "emit mqttDisconnected()!! from MQTT Thread.";
        emit mqttDisconnected();
        return;
    }

    MQTT::Message message;
    // QoS 0
    char buf[100];
    sprintf(buf, "Hello World!  QoS 0 message from app");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    mIsConnected = true;
    mRetry = 0;
    mArrivedcount = 0;
    forever {
        {
            QMutexLocker locker(&mutex);
            if (mStopped) {
                qDebug() << " MQTT Thread is stopped!!";
                break;
            }
        }
        rc = client.yield(500); // 500 ms
        if (rc != MQTT::SUCCESS) {
            qDebug() << " @@@@@@@@@@ MQTT yield failed";
            break;
        }

        if (!imageData.isEmpty()) {
            message.payload = (void*)imageData.data();
            message.payloadlen = imageData.size();

            rc = client.publish("terminal01/print", message);
            if (rc != 0) {
                printf("Error %d from sending QoS 0 message\n", rc);
            }
            else {
                imageData.clear();
            }
        }

        // pop queue and publish
    }

    rc = client.unsubscribe(topic.data());
    if (rc != 0)
        qDebug() << " @@@@@@@@@@ MQTT rc from unsubscribe was " << rc;

    rc = client.disconnect();
    if (rc != 0)
        qDebug() << " @@@@@@@@@@ MQTT rc from disconnect was " << rc;

    ipstack.disconnect();

    qDebug() << " @@@@@@@@@@ MQTT Finishing with " << mArrivedcount << " messages received.";

    {
        QMutexLocker locker(&mutex);
        mIsConnected = false;
        if (!mStopped) {
            qDebug() << "emit mqttDisconnected()!! from MQTT Thread.";
            emit mqttDisconnected();
        }
    }
}
