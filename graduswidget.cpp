#include "graduswidget.h"
#include "ui_graduswidget.h"
#include <QDebug>

GradusWidget::GradusWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GradusWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::SplashScreen);

    // задаем настройки
    cursettings = new MySettings();
    connect(cursettings,SIGNAL(applysignal()),this,SLOT(ApplyNewSettings()));
    cursettings->readall(); // читаем настройки из файла, если его нет - останутся по умолчанию

    setAction = new QAction("Настройки",this);
    connect(setAction,SIGNAL(triggered()),cursettings,SLOT(ShowSettingsForm()));

    aboutAction = new QAction("О программе", this);
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(showAboutWindow()));

    // set background image
    QPixmap bkgnd;
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    // right mouse click menu
    quitAction = new QAction("Выход", this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    m_pmnu = new QMenu(this);
    m_pmnu->addAction(setAction);
    m_pmnu->addAction(aboutAction);
    m_pmnu->addAction(quitAction);

    templabel = new QLabel(this);
    templabel->setGeometry(this->geometry());
    templabel->setAlignment(Qt::AlignCenter);

    myfont.setFamily("Arial");
    myfont.setPointSize(cursettings->fontsize);
    templabel->setFont(myfont);
    templabel->setStyleSheet("color: " + cursettings->fontcol.name() + ";");
    this->setWindowOpacity(cursettings->opacity);
    changecolor(cursettings->color1,cursettings->color2);


    manager = new QNetworkAccessManager(this);
    isAlarm = false;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerexpired()));
    url = QString(cursettings->url);
    timer->setInterval(cursettings->period*1000);
    timer->start();
    this->timerexpired();
}

void GradusWidget::showAlarm()
{
    templabel->setText("n/a");
}

void GradusWidget::showTemperature(QString string)
{
    templabel->setText(string + "°C");
}

void GradusWidget::replyFinished(QNetworkReply *reply) {
    QString statuscode = QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    if (statuscode == "200") {
        isAlarm = false;
        QByteArray rbytes = reply->readAll();
        QString temp(rbytes);
        int pos1 = temp.indexOf(cursettings->HTMLbefore);
        if (pos1 == -1) {
            isAlarm = true;
            showAlarm();
            return;
        }
        int len1 = cursettings->HTMLbefore.length();
        temp = temp.mid(pos1+len1);
        int pos2 = temp.indexOf(cursettings->HTMLafter);
        if (pos2 == -1) {
            isAlarm = true;
            showAlarm();
            return;
        }
        QString res = temp.mid(0,pos2);
        res=res.trimmed();
        showTemperature(res);

    }
    else {
        isAlarm = true;
        showAlarm();
    }
}

void GradusWidget::readdata(QUrl url)
{
    manager->get(QNetworkRequest(url));
}

void GradusWidget::timerexpired()
{
    readdata(QUrl(url));
}

void GradusWidget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(this->x() + event->x(), this->y() + event->y());
}

GradusWidget::~GradusWidget()
{
    delete ui;
}
