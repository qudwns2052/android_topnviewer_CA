#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thread.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHeaderView *verticalHeader2 = ui->tableWidget->verticalHeader();

#ifdef Q_OS_ANDROID
    verticalHeader2->setDefaultSectionSize(100);
#endif

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 70px; }");

    // start Server
    {
        system("export LD_PRELOAD=/system/lib/libfakeioctl.so");
        system("su -c \"nexutil -m2\"");
        system("su -c \"/data/local/tmp/gilgil/topnviewerServer&\"");
        sleep(1);
    }


    int server_port = 9998;

    //socket connection
    {
        if(!connect_sock(&client_sock, server_port))
        {
            printf("connection error\n");
            exit(1);
        }

        printf("connection ok\n");
    }


    QStringList label = {"Mac address", "Signal"};
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setHorizontalHeaderLabels(label);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    scanThread_ = new ScanThread(client_sock);

    QObject::connect(scanThread_, &ScanThread::captured, this, &MainWindow::processCaptured);

    scanThread_->start();
}

MainWindow::~MainWindow()
{
    system("su -c \"killall -9 topnviewerServer\"");
    delete ui;
}

void MainWindow::processCaptured(char* data)
{

    QString temp = QString(data);
    QStringList info = temp.split("\t");

    int row = ui->tableWidget->rowCount();

    if(info[0] == "1") // if ap info
    {
        return;
    }

    if(station_map.find(info[2]) != station_map.end())
    {
        if(station_map[info[2]].signal == info[3]) // if different signal
        {
            return;
        }

        station_map[info[2]].signal = info[3];

        for(int i=0; i<row; i++)
        {
            if(ui->tableWidget->item(i, 1)->text() == info[2])
            {
                ui->tableWidget->item(i, 2)->setText(info[3]);
//                QTableWidgetItem *item2 = new QTableWidgetItem(info[3]);
//                item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
//                ui->tableWidget->setItem(i, 1, item2);
                break;
            }
        }

    }
    else
    {
        station_map[info[2]].signal = info[3];
        station_map[info[2]].ap = info[1];


        ui->tableWidget->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem(info[1]);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, item);
        QTableWidgetItem *item2 = new QTableWidgetItem(info[2]);
        item2->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, item2);
        QTableWidgetItem *item3 = new QTableWidgetItem(info[3]);
        item3->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 2, item3);
    }

    ui->tableWidget->setSortingEnabled(true);

}

