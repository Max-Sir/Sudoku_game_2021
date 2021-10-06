#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Matrix m,prob,mark;//Matrix m is stored in binary mode, e.g. (100010010)2 means selecting number 1,5,8
protected:
    void load_prob(int clear);
};
#endif // MAINWINDOW_H
