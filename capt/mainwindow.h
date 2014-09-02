#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <list>
#include <iostream>
#include <Captcha.h>
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    QLabel *path;
    QLabel *ans;
    QLabel *captchatask;
    QLabel *header;
    QLabel *output;

    QLabel *img;
    QPixmap *upload;


    QString  file;
    QLineEdit *path2Text;
    QLineEdit *ans2text;
    QPushButton *breakCaptchaButton;
    QPushButton *browse;

  public slots:
    void DecodeCaptchaPushed();
    void browsePushed();

};

#endif // MAINWINDOW_H
