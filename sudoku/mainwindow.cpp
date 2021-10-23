#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFont>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QPalette>
#include <QKeyEvent>
#include <QPainter>
#include <QGroupBox>
#include <QTextCodec>
#include <QPainter>
#include <QDir>

static QFont gamefont("Ubuntu",16);
static QFont smallfont("Ubuntu",10);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    select_x=select_y=0;//current selected cell
    level=1;
    stage=0;
    history_temp=-1;
    wrong_state=0;
    state=0;
    ui->setupUi(this);
    //set blue buttons
    ui->pushButton->installEventFilter(this);
    ui->pushButton_2->installEventFilter(this);
    ui->pushButton_3->installEventFilter(this);
    ui->pushButton->setFocusPolicy(Qt::NoFocus); 
    ui->pushButton_2->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_3->setFocusPolicy(Qt::NoFocus);
    //set timer
    Timer=new QTimer;
    Time_record=new QTime(0,0,0,0);
    ui->timer->setDigitCount(8);
    ui->timer->setSegmentStyle(QLCDNumber::Flat);
    ui->timer->display(Time_record->toString("hh:mm:ss"));
    connect(Timer,SIGNAL(timeout()),this,SLOT(time_update()));
    //set number button
    num=new QPushButton*[10];
    for(int i=1,cnt=1;i<=3;++i)
        for(int j=1;j<=3;++j,++cnt)
        {
            num[cnt]=new QPushButton;
            QIcon icon;
            icon.addFile(QStringLiteral(":/fig/%1").arg(QString::number(cnt)));
            num[cnt]->setIcon(icon);
            num[cnt]->setFlat(true);
            num[cnt]->setAccessibleName(QString::number(cnt));
            num[cnt]->setFocusPolicy(Qt::NoFocus);
            ui->gridLayout_2->addWidget(num[cnt],i-1,j-1,1,1);
            connect(num[cnt],SIGNAL(clicked(bool)),this,SLOT(on_num_clicked()));
        }
    //set mark button and delete button
    QIcon icon,icon2;
    icon.addFile(QStringLiteral(":/fig/mark"));//marker button
    icon2.addFile(":fig/xxx");// delete button
    record_button=new QPushButton;
    record_button->setIcon(icon);
    record_button->setFlat(true);
    record_button->setFocusPolicy(Qt::NoFocus);
    connect(record_button,SIGNAL(clicked(bool)),this,SLOT(on_mark_button_clicked()));
    ui->gridLayout_2->addWidget(record_button,3,1,1,1);
    delete_button=new QPushButton;
    delete_button->setIcon(icon2);
    delete_button->setFlat(true);
    delete_button->setFocusPolicy(Qt::NoFocus);
    connect(delete_button,SIGNAL(clicked(bool)),this,SLOT(on_del_button_clicked()));
    ui->gridLayout_2->addWidget(delete_button,3,0,1,1);
    //set tablewidget
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setColumnCount(15);
    ui->tableWidget->setRowCount(15);
    ui->tableWidget->resize(404,404);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->installEventFilter(this);
    for(int i=0;i<15;++i)
    {
        ui->tableWidget->setRowHeight(i,44);
        ui->tableWidget->setColumnWidth(i,44);
    }
    //set bold line
    ui->tableWidget->setRowHeight(0,1);ui->tableWidget->setColumnWidth(0,1);
    ui->tableWidget->setRowHeight(4,1);ui->tableWidget->setColumnWidth(4,1);
    ui->tableWidget->setRowHeight(5,1);ui->tableWidget->setColumnWidth(5,1);
    ui->tableWidget->setRowHeight(9,1);ui->tableWidget->setColumnWidth(9,1);
    ui->tableWidget->setRowHeight(10,1);ui->tableWidget->setColumnWidth(10,1);
    ui->tableWidget->setRowHeight(14,1);ui->tableWidget->setColumnWidth(14,1);
    //set 81 button
    button=new QPushButton**[9];
    for(int i=0;i<9;++i)
    {
        button[i]=new QPushButton*[9];
        for(int j=0;j<9;++j)
        {
            button[i][j]=new QPushButton;
            //button[i][j]->setMinimumSize(20,20);
            button[i][j]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            button[i][j]->setFont(gamefont);
            button[i][j]->installEventFilter(this);
            ui->tableWidget->setBackgroundRole(QPalette::ColorRole());
            ui->tableWidget->setCellWidget(i<3?i+1:i<6?i+3:i+5,j<3?j+1:j<6?j+3:j+5,button[i][j]);
            button[i][j]->setAccessibleName(QString::number(i*9+j));
            connect(button[i][j],SIGNAL(clicked()),this,SLOT(on_button_clicked()));
            //QPalette pal=button[i][j]->palette();
            //if((i/3*3+j/3)&1)button[i][j]->setStyleSheet("color:black;background-color:lightgray");
            //else
            button[i][j]->setStyleSheet("color:black;background:white;");
            button[i][j]->setText(QString(" "));
            button[i][j]->setFlat(true);
            button[i][j]->setAutoFillBackground(true);
            button[i][j]->setDisabled(true);
            button[i][j]->setFocusPolicy(Qt::NoFocus);
        }
    }
    //setting of the media player (для проигрывания музыки)
    player=new QMediaPlayer;
    player->setMedia(QUrl("qrc:/fig/LuvLetter.wav"));
    show_info();
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
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
        {
            mark.mtr[i][j]=0;
            if(level==0)
                prob.mtr[i][j]=m.mtr[i][j]=0;
        }
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

void MainWindow::show_solution(int origin)
{
    if(origin)//no custom node
    {
        double c0=clock();
        sol.solve(prob,1);
        c0=(clock()-c0)/CLOCKS_PER_SEC*1000;
        ui->statusBar->showMessage(QStringLiteral("Solved. Using %1 ms").arg(c0));
    }
    else //custom mode
    if(wrong_state)
    {
        ui->statusBar->showMessage("No solution!");
        return;
    }
    else
    {
        Matrix tmp;
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j)
                if(sol.calc(m.mtr[i][j])==1)
                    tmp.mtr[i][j]=sol.idx[m.mtr[i][j]];
                else
                    tmp.mtr[i][j]=0;
        int ans=sol.solve(tmp,2);
        double c0=clock();
        sol.solve(tmp,1);
        c0=(clock()-c0)/CLOCKS_PER_SEC*1000;
        if(ans==0)
        {
            //qDebug()<<"No solution";
            ui->statusBar->showMessage(QStringLiteral("No solution! Using %1 ms").arg(c0));
            return;
        }
        else if(ans==2)
            ui->statusBar->showMessage(QStringLiteral("Multiple solution. Using %1 ms").arg(c0));
        else
            ui->statusBar->showMessage(QStringLiteral("Solved. Using %1 ms").arg(c0));
    }
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
        {
            if(prob.mtr[i][j]==0)
            {button[i][j]->setStyleSheet("color:green");}

            button[i][j]->setText(QString::number(sol.a.mtr[i][j]));
            button[i][j]->setFlat(true);
            button[i][j]->setFont(gamefont);
            button[i][j]->setFocusPolicy(Qt::NoFocus);
        }
}

void MainWindow::newgame()
{
    //init game
    //level=1;stage=1;
    if(level==1){
        if(stage==1)      prob.__init__("369.5247885.6749317143.826568392715459741682..21835697138769.4224.5837199.52413.6");
        else if(stage==2) prob.__init__("...2..5.32.37569..59..317.21349278569.56.34.16821453977.956..34..13726.93.8..9...");
        else if(stage==3) prob.__init__("1597.3..8786245.93.4219..6549.3..651537...942621..9.8787..3621.26.8145799..5.7836");
        else if(stage==4) prob.__init__("286.15.4...467325.37.2.8.6.7.91246..6..387..5..85967.1.9.4.1.72.678529...4.73.816");
        else              sol.generate_range(20,36,1),prob=sol.a;
    }else if(level==2){
        if(stage==1)      prob.__init__("7...8...94.1..625...6.3..14.6..59.....9...1.....74..2.85..9.6...923..8.76...1...2");
        else if(stage==2) prob.__init__("4....1.5.9.18.7.6.5.82.41...5..9.......7.5.......1..2...75.29.6.8.1.62.5.6.3....7");
        else if(stage==3) prob.__init__("......1.....63.95.63..41.2...32.9...72.....91...8.37...4.12..89.95.64.....1......");
        else if(stage==4) prob.__init__("......87.2....7.51.583..9.2..15..7.9...9.8...9.3..25..6.7..341.12.7....8.34......");
        else              sol.generate_range(42,48,1),prob=sol.a;
    }else if(level==3){
        if(stage==1)      prob.__init__("6...3495...3.28...2......6.1.98..7.............2..94.1.2......4...27.5...3749...6");
        else if(stage==2) prob.__init__(".31.97.6....6......2.....18.....5.29..2.7.5..95.1.....17.....3......9....9.86.24.");
        else if(stage==3) prob.__init__("..7..13.9..6..8...4.3.7..1.1...2..3...........4..3...6.5..1.6.8...4..5..2.95..4..");
        else if(stage==4) prob.__init__("..1....986...28.....8.76..1....9..25...4.7...59..6....7..24.9.....71...243....1..");
        else              sol.generate_range(53,57,1),prob=sol.a;
    }else if(level==4){
        if(stage==1)      prob.__init__("8..........36......7..9.2...5...7.......457.....1...3...1....68..85...1..9....4..");
        else if(stage==2) prob.__init__("16....7..7...3.......4...2.....173..9.......8..452.....1...6.......5...9..9....35");
        else if(stage==3) prob.__init__(".3...58..6...9...........73...5..43...1...5...48..9...19...........3...5..57...1.");
        else if(stage==4) prob.__init__("..2..7..4.....9.8..54.........47.8...6.....7...1.36.........16..3.9.....4..3..7..");
        else              sol.generate_range(58,81,1),prob=sol.a;
    }
    else                  prob.__init__(".................................................................................");
    sol.solve(prob,1);
    prob.print();
    load_prob(1);
    state=1;
    timer_start();
}

QString MainWindow::change_bin_number_into_String(int x)//change binary number into string
{
    QString s;
    int cnt=0;
    if(sol.calc(x)==1)
    {
        s.append(sol.idx[x]+'0');
        cnt=1;
    }
    else{
        for(int i=1;i<=9;++i)
          {
            if(x&(1<<i))
            {
                if(cnt%2==1)s.append(QString(' '));
                if(cnt==2)
                    s.append(QString('\n'));
                s.append(QString::number(i));
                cnt++;
            }
          }
    }
    if(cnt==0){s.append(' ');}
    return s;
}

void MainWindow::pause()
{
    if(state!=1 || level==0)return;
    QIcon icon;
    icon.addFile(":/fig/play");
    ui->actionPause->setIcon(icon);
    state=2;
    Timer->stop();
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
        {
            button[i][j]->setText(" ");
            button[i][j]->setDisabled(true);
            button[i][j]->setFlat(true);
            button[i][j]->setStyleSheet("background-color:white");
            button[i][j]->setAutoFillBackground(true);
            button[i][j]->setAutoDefault(false);
            button[i][j]->setDefault(false);
        }
}

void MainWindow::continuegame()
{
    if(state!=2||level==0)return;
    QIcon icon;
    icon.addFile(":fig/pause");
    ui->actionPause->setIcon(icon);
    state=1;
    Timer->start(1000);
    refresh_xy(0);
}

void MainWindow::show_tip()
{
    if(level==0)return;
    int minn=1000;
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(prob.mtr[i][j]==0 && (1<<sol.a.mtr[i][j])!=m.mtr[i][j] && sol.showtime.mtr[i][j]<minn)
            {
                minn=sol.showtime.mtr[i][j];
                select_x=i;
                select_y=j;
            }
    if(minn==1000)return;
    minn=m.mtr[select_x][select_y];
    m.mtr[select_x][select_y]=1<<sol.a.mtr[select_x][select_y];
    refresh_xy(0);
    m.mtr[select_x][select_y]=minn;
}

int MainWindow::checkavailable(int i,int j)
{
    for(int k=0;k<9;++k)
        if((i!=k && m.mtr[k][j]==m.mtr[i][j]) || (j!=k && m.mtr[i][k]==m.mtr[i][j]))
            return 0;
    for(int dx=i/3*3,dy=j/3*3,x=0;x<3;++x)
        for(int y=0;y<3;++y)
            if((i!=x+dx||j!=y+dy)&&m.mtr[i][j]==m.mtr[x+dx][y+dy])
                return 0;
    return 1;
}

void MainWindow::refresh_xy(int add)
{
    if(add && (m!=history[history.length()-1] || mark!=hmark[hmark.length()-1]))
    {
        history_temp=history.length();
        history.append(m);
        hx.append(select_x);
        hy.append(select_y);
        hmark.append(mark);
    }
    //set normal grid
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(prob.mtr[i][j])
            {//здесь начальные клетки поля чёрные black
                button[i][j]->setText(change_bin_number_into_String(m.mtr[i][j]));
                button[i][j]->setStyleSheet("color:black;background-color:white");
                button[i][j]->setFlat(true);
                button[i][j]->setFont(gamefont);
            }
            else
            {   //вставленные пользователем правильные клетки под условие green
                button[i][j]->setStyleSheet("color:green;background-color:white");
                button[i][j]->setFont(gamefont);
                if(m.mtr[i][j])
                {
                    if(sol.calc(m.mtr[i][j])>1)
                    {//число чисел в ячейке>1 gray
                        button[i][j]->setStyleSheet("color:gray;background-color:white");
                        button[i][j]->setFont(smallfont);
                    }
                    button[i][j]->setText(change_bin_number_into_String(m.mtr[i][j]));
                }
                else
                {
                    button[i][j]->setText(QString(" "));
                    button[i][j]->setAutoFillBackground(true);
                }
                button[i][j]->setFlat(true);
            }
    //set highlight grid текужая активная клетка если подходит по условию blue
    button[select_x][select_y]->setStyleSheet("color:blue;background-color:white");
    button[select_x][select_y]->setFocusPolicy(Qt::NoFocus);
    if(m.mtr[select_x][select_y]==0)
        for(int i=0;i<9;++i)
        {
            button[i][select_y]->setFlat(true);
            button[select_x][i]->setFlat(true);
        }
    else if(sol.calc(m.mtr[select_x][select_y])==1)
    {
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j)
                if(m.mtr[i][j]==m.mtr[select_x][select_y])
                    button[i][j]->setFlat(true);
    }
    //set wrong grid не подходит под условие заполнения значит red
    int wrong_cnt=0,acc_cnt=0;
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(prob.mtr[i][j]==0 && sol.calc(m.mtr[i][j])==1)
            {
                acc_cnt++;
                if(checkavailable(i,j)==0)
                {
                    button[i][j]->setStyleSheet("color:red;background-color:white");
                    wrong_cnt++;
                }
            }
            else if(prob.mtr[i][j]&&checkavailable(i,j)==0)
                wrong_cnt++;
    if(wrong_cnt){wrong_state=1;}
    else{ wrong_state=0;}
    button[select_x][select_y]->setFlat(true);
    button[select_x][select_y]->setAutoFillBackground(false);
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
        {
            button[i][j]->setFocusPolicy(Qt::NoFocus);
            button[i][j]->setDisabled(false);
            button[i][j]->setAutoDefault(false);
            button[i][j]->setDefault(false);
            button[i][j]->setAutoExclusive(false);
        }
    button[select_x][select_y]->setAutoDefault(true);
    button[select_x][select_y]->setDefault(true);
    QIcon Ic0,Ic1;
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(mark.mtr[i][j])
            {
                Ic1.addFile(QStringLiteral(":/fig/mark"));
                button[i][j]->setIcon(Ic1);
            }
            else
                button[i][j]->setIcon(Ic0);
    QString px("ABCDEFGHI"),py("123456789");
    ui->statusBar->showMessage("("+px[select_x]+","+py[select_y]+")");
    //update();
    if(wrong_cnt==0 && acc_cnt==prob.count_0() && level)
        finish();
}

void MainWindow::addnumber(int num)
{
    if(level)
    {
        if(prob.mtr[select_x][select_y]==0)
        {
            m.mtr[select_x][select_y]^=1<<num;
            if(sol.calc(m.mtr[select_x][select_y])>4)
                m.mtr[select_x][select_y]^=1<<num;
            else
            {
                button[select_x][select_y]->setText(change_bin_number_into_String(m.mtr[select_x][select_y]));
                refresh_xy(1);
            }
        }
    }
    else//custom mode
    {
        int tmp1=prob.mtr[select_x][select_y];
        int tmp2=m.mtr[select_x][select_y];
        prob.mtr[select_x][select_y]=num;
        m.mtr[select_x][select_y]=1<<num;
        refresh_xy(0);
        if(wrong_state)
        {
            prob.mtr[select_x][select_y]=tmp1;
            m.mtr[select_x][select_y]=tmp2;
        }
        refresh_xy(1);
    }
}

void MainWindow::deletegrid()
{
    if(level==0){//custom mode
        prob.mtr[select_x][select_y]=0;
        m.mtr[select_x][select_y]=0;
    }
    else if(prob.mtr[select_x][select_y]==0){
        m.mtr[select_x][select_y]=0;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{//qDebug()<<"key pressed";
    if(state!=1)return;
    if(e->key()>=Qt::Key_1 && e->key()<=Qt::Key_9)
    {
        addnumber(e->key()-Qt::Key_1+1);
        return;
    }
    if(e->key()==Qt::Key_M)
    //if(e->key()==Qt::Key_Space||e->key()==Qt::Key_Tab)
    {
        mark_grid();
        return;
    }
    switch (e->key()){
    case Qt::Key_Delete:
        deletegrid();
        break;
    case Qt::Key_Up:
    case Qt::Key_I:
    case Qt::Key_W:
        select_x=(select_x+8)%9;
//        qDebug()<<"key up";
        break;
    case Qt::Key_Down:
    case Qt::Key_K:
    case Qt::Key_S:
        select_x=(select_x+1)%9;
//        qDebug()<<"key down";
        break;
    case Qt::Key_Left:
    case Qt::Key_J:
    case Qt::Key_A:
        select_y=(select_y+8)%9;
//        qDebug()<<"key left";
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
    case Qt::Key_D:
        select_y=(select_y+1)%9;
//        qDebug()<<"key right";
        break;
    default:
        QWidget::keyPressEvent(e);
    }
    refresh_xy(0);
}

void MainWindow::on_button_clicked()
{
    if(state!=1)return;
    QPushButton*btn=qobject_cast<QPushButton*>(sender());
    int id=btn->accessibleName().toInt();
    select_x=id/9;
    select_y=id%9;
    refresh_xy(0);
}

void MainWindow::on_num_clicked()
{
    if(state!=1)return;
    QPushButton*btn=qobject_cast<QPushButton*>(sender());
    addnumber(btn->accessibleName().toInt());
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
    mes+="    Custom mode: Вы можете ввести своё судоку для получения решения/ответа;\n";
    mes+="\n";
    mes+="Эта программа гарантирует, что поля судоку в режимах Easy/Normal/Hard/Crazy имеют только одно решение.\n";
    mes+="\n";
    mes+="    Клавиши W/A/S/D or I/J/K/L: Up/Left/Down/Right;\n";
    mes+="    Клавиши 1-9: добавить число в выбранную клетку;\n";
    mes+="    Key M: установить флаг в выбранную клетку;\n";
    mes+="    Клавиша Del: удалить все значения в выбранной ячейке;\n";
    mes+="    Клавиша Пробел: пауза/продолжить;\n";
    mes+="    Ctrl+I: Custom mode (Пользовательский режим);\n";
    mes+="    Ctrl+Z: Шаг назад;\n";
    mes+="    Ctrl+Y: Шаг вперёд;\n";
    mes+="    Ctrl+M: Включить/Остановить музыку;\n";
    mes+="    Alt+I: Показать данное окно;\n";
    mes+="    Ctrl+R: новое Random Crazy поле;\n";
    mes+="\nНаслаждайтесь!\n";
    QMessageBox::information(this,tr("Игра Судоку сделанная студентами гр.950501"),mes);
}

void MainWindow::on_actionInfo_triggered()
{
    show_info();
}

void MainWindow::finish()
{
    state=3;
    if(level==0)return;
    Timer->stop();
    QString s=Time_record->toString("hh:mm:ss");
    s="    Вы решили этот пазл \nза "+s;
    QMessageBox::information(this,tr("Поздравляю с решением!"),s,tr("Продолжить"));
}

void MainWindow::undo()
{
    if(history_temp<=0)
        return;
    m=history[--history_temp];
    select_x=hx[history_temp];
    select_y=hy[history_temp];
    mark=hmark[history_temp];
    if(level==0)
    {
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j)
                if(m.mtr[i][j])
                    prob.mtr[i][j]=sol.idx[m.mtr[i][j]];
                else
                    prob.mtr[i][j]=0;
    }
    refresh_xy(0);
}

void MainWindow::redo()
{
    if(history_temp>=history.length()-1)
        return;
    m=history[++history_temp];
    select_x=hx[history_temp];
    select_y=hy[history_temp];
    mark=hmark[history_temp];
    if(level==0)
    {
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j)
                if(m.mtr[i][j])
                    prob.mtr[i][j]=sol.idx[m.mtr[i][j]];
                else
                    prob.mtr[i][j]=0;
    }
    refresh_xy(0);
}

void MainWindow::timer_start()
{
    Time_record->setHMS(0,0,0,0);
    ui->timer->display(Time_record->toString("hh:mm:ss"));
    if(level==0)return;
    Timer->start(1000);
}

void MainWindow::time_update()
{
    if(level==0)return;
    *Time_record=Time_record->addSecs(1);
    ui->timer->display(Time_record->toString("hh:mm:ss"));
}

void MainWindow::mark_grid()
{
    mark.mtr[select_x][select_y]^=1;
    refresh_xy(1);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(stage)stage=(stage+1)%5;
    newgame();
}

void MainWindow::on_actionReset_triggered()
{
    if(state!=1)return;
    load_prob(0);
}

void MainWindow::on_actionSolve_triggered()
{
    if(state!=1)return;
    show_solution(0);
}

void MainWindow::on_pushButton_3_clicked()
{
    if(state!=1)return;
    if(level)
        show_solution(1);
    else
        show_solution(0);
}

void MainWindow::on_pushButton_clicked()
{
    if(state!=1)return;
    show_tip();
}

void MainWindow::on_actionPause_triggered()
{
    if(state==1)
        pause();
    else if(state==2)
        continuegame();
}

void MainWindow::on_actionStart_triggered()
{
    continuegame();
}

void MainWindow::on_actionUndo_triggered(){if(state!=1)return;undo();}
void MainWindow::on_actionRedo_triggered(){if(state!=1)return;redo();}

void MainWindow::on_mark_button_clicked()
{
    if(state!=1)return;
    mark_grid();
}

void MainWindow::on_del_button_clicked()
{
    //qDebug()<<"delete "<<state;
    if(state!=1)return;
    deletegrid();
    refresh_xy(1);
}

void MainWindow::on_actionLv_1_triggered(){level=1;stage=1;newgame();}
void MainWindow::on_actionLv_2_triggered(){level=1;stage=2;newgame();}
void MainWindow::on_actionLv_3_triggered(){level=1;stage=3;newgame();}
void MainWindow::on_actionLv_4_triggered(){level=1;stage=4;newgame();}
void MainWindow::on_actionLv_5_triggered(){level=2;stage=1;newgame();}
void MainWindow::on_actionLv_6_triggered(){level=2;stage=2;newgame();}
void MainWindow::on_actionLv_7_triggered(){level=2;stage=3;newgame();}
void MainWindow::on_actionLv_8_triggered(){level=2;stage=4;newgame();}
void MainWindow::on_actionLv_9_triggered(){level=3;stage=1;newgame();}
void MainWindow::on_actionLv_10_triggered(){level=3;stage=2;newgame();}
void MainWindow::on_actionLv_11_triggered(){level=3;stage=3;newgame();}
void MainWindow::on_actionLv_12_triggered(){level=3;stage=4;newgame();}
void MainWindow::on_actionLv_13_triggered(){level=4;stage=1;newgame();}
void MainWindow::on_actionLv_14_triggered(){level=4;stage=2;newgame();}
void MainWindow::on_actionLv_15_triggered(){level=4;stage=3;newgame();}
void MainWindow::on_actionLv_16_triggered(){level=4;stage=4;newgame();}
void MainWindow::on_actionRandom_triggered(){level=1;stage=0;newgame();}
void MainWindow::on_actionRandom_2_triggered(){level=2;stage=0;newgame();}
void MainWindow::on_actionRandom_3_triggered(){level=3;stage=0;newgame();}
void MainWindow::on_actionRandom_4_triggered(){level=4;stage=0;newgame();}
void MainWindow::on_actionInput_Sudoku_Game_triggered(){level=stage=0;newgame();}

void MainWindow::on_actionMusic_triggered()
{
    if(player->state()==QMediaPlayer::PlayingState)
    {//turn off
        QIcon icon;
        icon.addFile(":/fig/mute");
        ui->actionMusic->setIcon(icon);
        player->pause();
    }
    else
    {//turn on
        QIcon icon;
        icon.addFile(":/fig/music");
        ui->actionMusic->setIcon(icon);
        player->setVolume(30);
        player->play();
    }
}
