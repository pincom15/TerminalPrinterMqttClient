#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "mqttthread.h"

extern QByteArray imageData;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *buttonFileOpen;
    QImage scaled;
    QImage imageToProcess;
    double getGray(int x, int y);
    MqttThread mqttThread;
    int count = 0;


private slots:
    void imageOpen();
    void imageSave();
    void imagePrint();
    void refresh();
    void change(int k);
    void setTerminalStatus(QString);
};

#endif // MAINWINDOW_H
