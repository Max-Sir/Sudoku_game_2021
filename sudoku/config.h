#ifndef CONFIG
#define CONFIG

#include <QObject>
#include <QDebug>
#include <QString>
#include <ctime>

extern const int position[9][9];

class Matrix{
public:
	int mtr[9][9];
    

    void print(){
        QString s;
        for(int i=0;i<9;++i)
        {
            for(int j=0;j<9;++j)
            {
                if(mtr[i][j]==0) s+='.';
                else s+=mtr[i][j]+'0';
            }
        }
            qDebug()<<s;
    }
};


#endif // CONFIG}

