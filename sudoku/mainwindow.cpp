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

void MainWindow::show_info()
{
    QString mes;
    mes+="\nСпасибо за то что играете в данную игру.\n";
    mes+="Подсказки:\n";
    mes+="    Новая игра (Alt+N): выберите уровень сложности и уровень;\n";
    mes+="    Easy mode:   20~36 заполненных ячеек;\n";
    mes+="    Normal mode: 42~48 заполненных ячеек;\n";
    mes+="    Hard mode:   53~57 заполненных ячеек;\n";
    mes+="    Crazy mode:  58~81 заполненных ячеек, **Наберитесь терпения**;\n";  
    mes+="\n";
    mes+="Эта программа гарантирует, что поля судоку в режимах Easy/Normal/Hard/Crazy имеют только одно решение.\n";
    mes+="\nНаслаждайтесь!\n";
    
}
