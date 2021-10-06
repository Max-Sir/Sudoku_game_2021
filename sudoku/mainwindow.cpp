#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_prob(int clear)
{
    if(clear)
    {
        //clear all history record
        history_temp=0;
        history.clear();
        history.append(m);
        hx.clear();
        hx.append(select_x);
        hy.clear();
        hy.append(select_y);
        hmark.clear();
        hmark.append(mark);
    }
    refresh_xy(1);
}
