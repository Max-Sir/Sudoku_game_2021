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
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
        {
            if(prob.mtr[i][j])
                m.mtr[i][j]=1<<prob.mtr[i][j];
            else
                m.mtr[i][j]=0;
        }
}
