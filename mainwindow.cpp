#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QBuffer>

QByteArray imageData;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(released()), this, SLOT(imageOpen()));
    connect(ui->pushButtonSave, SIGNAL(released()), this, SLOT(imageSave()));
    connect(ui->pushButtonPrint, SIGNAL(released()), this, SLOT(imagePrint()));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(change(int)));

    connect(&mqttThread, SIGNAL(messageReceived(QString)), this, SLOT(setTerminalStatus(QString)));

    ui->statusBar->showMessage("     뭐가 잘안되면 주문중계팀 박진용에게 갠라주세요. (라인ID:pincom15)", 60000);
    mqttThread.start();
}

MainWindow::~MainWindow()
{
    mqttThread.stop();

    delete ui;
}

void MainWindow::setTerminalStatus(QString status)
{
    QString statusText = QString::fromUtf8("단말기 상태 : ");

    if (status.contains("wait")) {
        ui->pushButtonPrint->setEnabled(true);
        statusText += QString::fromUtf8("온라인");
    }
    else if (status.contains("offline")) {
        ui->pushButtonPrint->setEnabled(false);
        statusText += QString::fromUtf8("오프라인");
    }
    else if (status.contains("printing")) {
        ui->pushButtonPrint->setEnabled(false);
        statusText += QString::fromUtf8("인쇄중");
    }
    else {
        ui->pushButtonPrint->setEnabled(false);
        statusText += QString::fromUtf8("알수없음");
    }

    ui->statusBar->showMessage(statusText);
}

void MainWindow::change(int k) {

    double factor = (double)k / 50;
    QRgb c;
    int r, g, b;
    for (int l = 0; l < scaled.height() - 1; ++l) {
        for (int k = 1; k < scaled.width() - 1; ++k) {
            c = QRgb(scaled.pixel(k,l));
            QColor col(c);
            col.getRgb(&r,&g,&b);
            double r_n = r / 255.0; r_n -= 0.5; r_n *= factor; r_n += 0.5; r_n *= 255;
            double g_n = g / 255.0; g_n -= 0.5; g_n *= factor; g_n += 0.5; g_n *= 255;
            double b_n = b / 255.0; b_n -= 0.5; b_n *= factor; b_n += 0.5; b_n *= 255;
            if(r_n>255) r_n=255; if(r_n<0) r_n=0;
            if(g_n>255) g_n=255; if(g_n<0) g_n=0;
            if(b_n>255) b_n=255; if(b_n<0) b_n=0;
            //imageToProcess.setPixel(k,l,qRgb((int)r_n,(int)g_n,(int)b_n));
            int gray = qGray((int)r_n,(int)g_n,(int)b_n);
            imageToProcess.setPixel(k,l,qRgb(gray,gray,gray));
        }
    }

    /*
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "JPG");

    qDebug() << "buffer size:" << buffer.size();
    */

    refresh();
}

double MainWindow::getGray(int x, int y) {
    int r, g, b;
    QRgb c = QRgb(imageToProcess.pixel(x, y));
    QColor col(c);
    col.getRgb(&r,&g,&b);

    return qGray((int)r,(int)g,(int)b);
}

void MainWindow::imageOpen() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/Users/jyp/Desktop", tr("Image Files (*.png *.jpg *.bmp)"));
    qDebug() << fileName << "hi";
    QImage image = QImage(fileName);
    scaled = image.scaled(576, 1600, Qt::KeepAspectRatio);
    imageToProcess = scaled.copy();

    change(ui->horizontalSlider->value());
    refresh();

}

void MainWindow::imageSave() {
    imageToProcess.convertToFormat(QImage::Format_Mono).save("image.jpg");
}

void MainWindow::imagePrint() {
    QByteArray msg = QByteArray("hello").append(QString::number(count++));
    qDebug() << "print:" << msg;

    mqttThread.setImageData(msg);
}

void MainWindow::refresh() {

    QPixmap pixmap = QPixmap::fromImage(imageToProcess.convertToFormat(QImage::Format_Mono));
    ui->label->setPixmap(pixmap);
    ui->label->show();

}
