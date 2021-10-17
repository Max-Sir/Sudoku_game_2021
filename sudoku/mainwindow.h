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
    void newgame();
    void pause();
    void continuegame();
    void load_prob(int clear);
    void show_solution(int origin);
    void refresh_xy(int add);
    void show_tip();
    int checkavailable(int,int);
    QString change_bin_number_into_String(int);
    void addnumber(int);
    void deletegrid();
    void show_info();
    void finish();

private slots:
    void on_actionInfo_triggered();
    void on_actionLv_1_triggered();
    void on_actionLv_2_triggered();
    void on_actionLv_3_triggered();
    void on_actionLv_4_triggered();
    void on_actionLv_5_triggered();
    void on_actionLv_6_triggered();
    void on_actionLv_7_triggered();
    void on_actionLv_8_triggered();
    void on_actionLv_9_triggered();
    void on_actionLv_10_triggered();
    void on_actionLv_11_triggered();
    void on_actionLv_12_triggered();
    void on_actionLv_13_triggered();
    void on_actionLv_14_triggered();
    void on_actionLv_15_triggered();
    void on_actionLv_16_triggered();
    void on_actionRandom_triggered();
    void on_actionRandom_2_triggered();
    void on_actionRandom_3_triggered();
    void on_actionRandom_4_triggered();
    void on_actionInput_Sudoku_Game_triggered();
    void on_actionMusic_triggered();
};
#endif // MAINWINDOW_H
