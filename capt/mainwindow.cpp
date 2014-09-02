#include "mainwindow.h"
#include<fstream>

using namespace std;
string main12(string x){
        string imageSource = x;
        CImg<unsigned char> sourceImage(imageSource.c_str());
        Captcha ans(sourceImage);
        int l = x.length();
        char type='1';
        for (int i=l-1 ; i>0;i--){
            if (x[i]=='/'){
                type = x[i-1];
                break;
            }
        }

        if (type=='2'||type=='4'){
            ans.decompose(5);
                  ans.crop();
        return ans.prediction();
        }
        if (type=='3'){
            //cout<<"3";
            ans.filter(3,1,3,1500);
            ans.decompose(50);
                  ans.crop();
        return ans.prediction();
        }
        
            ans.filter(2,2.5,3,15);
            ans.decompose(50);
                  ans.crop();
        return ans.prediction();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{


    QWidget *window = new QWidget;

    path  = new QLabel("Path",this);
    ans = new QLabel("Answer",this);
    captchatask = new QLabel("Break Captcha",this);
    header = new QLabel("Captcha-Decaptcha",this);
    output = new QLabel("Welcome:)",this);




    path2Text = new QLineEdit(this);
    ans2text = new QLineEdit(this);
       breakCaptchaButton = new QPushButton("DECODE",this);

    browse = new QPushButton("Browse",this);



    breakCaptchaButton->setMaximumWidth(100);

    img = new QLabel(this);
    //upload = new Pixmap;


    //upload = new QPixmap(this);
       connect(breakCaptchaButton,SIGNAL(clicked()),this,SLOT(DecodeCaptchaPushed()));
    connect(browse,SIGNAL(clicked()),this,SLOT(browsePushed()));


    QSpacerItem *spacerItem = new QSpacerItem(300, 20, QSizePolicy::Expanding);
    QHBoxLayout *lay1 = new QHBoxLayout;
    lay1->addWidget(path);
    lay1->addWidget(path2Text);
    lay1->addWidget(browse);


    QHBoxLayout *lay2 = new QHBoxLayout;
    QHBoxLayout *lay3 = new QHBoxLayout;
    lay2->addWidget(breakCaptchaButton,Qt::AlignCenter);
    lay3->addWidget(ans);
    lay3->addWidget(ans2text);
    QHBoxLayout *lay4 = new QHBoxLayout;
    lay4->addWidget(img);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(output,Qt::AlignCenter);
    layout->addWidget(header,Qt::AlignCenter);
    layout->addItem(spacerItem);
    layout->addWidget(captchatask,Qt::AlignLeft);
    layout->addLayout(lay1);
    layout->addLayout(lay2);
    layout->addLayout(lay3);
    layout->addLayout(lay4);
    layout->addItem(spacerItem);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

}


void MainWindow::DecodeCaptchaPushed()
{

    QString s=path2Text->text();
  string x= main12(s.toStdString());
  QString ans(x.c_str());
  ans2text->setText(ans);
   path2Text->clear();
}

void MainWindow::browsePushed()
{ img->setPixmap(NULL);
  file = QFileDialog::getOpenFileName(this);
  path2Text->setText(file);
   QString s=path2Text->text();
    upload = new QPixmap;
   upload->load(s);
   img->setPixmap(*upload);
   QPixmap temp = upload->scaledToHeight((img->height())*2 +5);
   img->setPixmap(temp);
   img->show();

}


