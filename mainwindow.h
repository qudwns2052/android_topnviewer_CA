#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include <QScrollBar>
#include "socket.h"
#include "thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


typedef struct station_map_info
{
    QString ap;
    QString signal;
}station_map_info;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int client_sock;
    char data[BUF_SIZE];
    char buf[BUF_SIZE];

    QMap <QString, station_map_info> station_map;

    ScanThread * scanThread_;

private slots:
    void processCaptured(char* data);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
