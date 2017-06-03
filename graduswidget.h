#ifndef GRADUSWIDGET_H
#define GRADUSWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QLabel>
#include <QSettings>
#include <QGridLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDesktopWidget>
#include <QColorDialog>
#include <QApplication>

#define PRINTER(name) (#name)
#define params(name) saveval((#name),name)
#define gparams(name) getval(("settings/"#name),name)

// класс, хранящий настройки программы
class MySettings : public QObject
{
    Q_OBJECT

public:
    // в пустом конструкторе задаем значения по умолчанию
    MySettings() : period(20), url("http://ns.krppc.ru:30000/main.html"), fontcol(QColor(255,255,255,0)), color1(QColor(231, 136, 26, 255)), color2(QColor(234, 116, 26, 255)), opacity(0.7), fontsize(50), width(176), height(150), HTMLbefore("<td>Temperature</td><td>"), HTMLafter("' C"), configfile(QApplication::applicationDirPath() + "/settings.conf") { settings = new QSettings(configfile,QSettings::IniFormat); }
    // в этом конструкторе значения задаются напрямую
    MySettings(int per, QString ur, QColor fontc, QColor col1, QColor col2, qreal opac, int fsize, int wid, int hei, QString htmlb, QString htmla, QString confile) : period(per), url(ur), fontcol(fontc), color1(col1), color2(col2), opacity(opac), fontsize(fsize), width(wid), height(hei), HTMLbefore(htmlb), HTMLafter(htmla), configfile(confile) { settings = new QSettings(configfile,QSettings::IniFormat); }
    ~MySettings() {}
    int period;
    QString url;
    QColor fontcol;
    QColor color1;
    QColor color2;
    qreal opacity;
    int fontsize;
    int width;
    int height;
    QString HTMLbefore;
    QString HTMLafter;
    QString configfile;
    QSettings *settings;
    QColor tempcol1; // временные переменные (нужны на случай отмены изменения настроек)
    QColor tempcol2; // -//-
    QColor tempcolfont;
    // GUI controls
    QGridLayout *grid;
    QSpinBox *spb1;
    QLineEdit *led1;
    QPushButton *pb1;
    QPushButton *pb2;
    QPushButton *fpb;
    QLineEdit *hled1;
    QLineEdit *hled2;
    QSpinBox *spb2;
    QSpinBox *spb3;
    QSpinBox *spb4;
    QSpinBox *spb5;
    QLineEdit *led2;
    QDialog *settingsform;

    // сохранить все параметры (установленные на данный момент значения членов класса) в файл настроек
    void saveall() {
        params(period);
        params(url);
        saveval("fontcol",fontcol.name());
        saveval("color1",color1.name());
        saveval("color2",color2.name());
        params(opacity);
        params(fontsize);
        params(width);
        params(height);
        params(HTMLbefore);
        params(HTMLafter);
        params(configfile);
    }

    // чтение всех настроек из файла (при неудаче остаётся значение по умолчанию)
    void readall()
    {
        period = gparams(period);
        url = gparams(url);
        fontcol = getcval("fontcol",fontcol.name());
        color1 = getcval("color1",color1.name());
        color2 = getcval("color2",color2.name());
        opacity = gparams(opacity);
        fontsize = gparams(fontsize);
        width = gparams(width);
        height = gparams(height);
        HTMLbefore = gparams(HTMLbefore);
        HTMLafter = gparams(HTMLafter);
        configfile = gparams(configfile);
    }

    // прямая запись настройки в файл, пример записи: saveval("period",20);
    void saveval(QString param, QVariant value)
    {
        settings->setValue("settings/" + param,value);
    }

    // чтение значения параметра
    int getval(QString param, int defaultVal=0) {
        return settings->value(param,defaultVal).toInt();
    }

    // цвета сохраняются в текстовом виде по их hex-имени, и читаются аналогично
    QColor getcval(QString param, QString defaultVal="#ffffff") {
        return QColor(settings->value("settings/" + param,defaultVal).toString());
    }

    qreal getval(QString param, qreal defaultVal=0.0)
    {
        return settings->value(param,defaultVal).toReal();
    }

    QString getval(QString param, QString defaultVal="") {
        return settings->value(param,defaultVal).toString();
    }

    void printvar() {
        qDebug() << PRINTER(url);
    }

signals:
    void applysignal();

public slots:

    // сгенерировать и показать форму для установки пользователем всех настроек
    void ShowSettingsForm()
    {
        settingsform = new QDialog();
        settingsform->setAttribute(Qt::WA_DeleteOnClose);
        settingsform->setWindowTitle("Форма настроек программы");
        // элементы будет размещать по сетке - слева подпись, справа - элемент настройки
        grid = new QGridLayout(settingsform);

        QLabel *lb1 = new QLabel("Период опроса температуры",settingsform);
        grid->addWidget(lb1,0,0);
        spb1 = new QSpinBox(settingsform);
        spb1->setRange(10,999999);
        spb1->setValue(period);
        grid->addWidget(spb1,0,1);

        QLabel *lb2 = new QLabel("Адрес для подключения",settingsform);
        grid->addWidget(lb2,1,0);
        led1 = new QLineEdit(settingsform);
        led1->setText(url);
        grid->addWidget(led1,1,1);

        QLabel *lb3 = new QLabel("Цвет 1 для градиента",settingsform);
        grid->addWidget(lb3,2,0);
        pb1 = new QPushButton(settingsform);
        pb1->setText("Цвет 1");
        QObject::connect(pb1,SIGNAL(clicked(bool)),this,SLOT(color1button()));
        grid->addWidget(pb1,2,1);

        QLabel *lb4 = new QLabel("Цвет 2 для градиента",settingsform);
        grid->addWidget(lb4,3,0);
        pb2 = new QPushButton(settingsform);
        pb2->setText("Цвет 2");
        QObject::connect(pb2,SIGNAL(clicked(bool)),this,SLOT(color2button()));
        grid->addWidget(pb2,3,1);

        QLabel *lb5 = new QLabel("Степень прозрачности окна, %",settingsform);
        grid->addWidget(lb5,4,0);
        spb2 = new QSpinBox(settingsform);
        spb2->setRange(0,100);
        spb2->setValue(opacity*100);
        grid->addWidget(spb2,4,1);

        QLabel *lb6 = new QLabel("Размер шрифта", settingsform);
        grid->addWidget(lb6,5,0);
        spb3 = new QSpinBox(settingsform);
        spb3->setRange(0,100);
        spb3->setValue(fontsize);
        grid->addWidget(spb3,5,1);

        QLabel *lb7 = new QLabel("Ширина основного окна", settingsform);
        grid->addWidget(lb7,6,0);
        spb4 = new QSpinBox(settingsform);
        QDesktopWidget qscr;
        spb4->setRange(0,qscr.availableGeometry().width());
        spb4->setValue(width);
        grid->addWidget(spb4,6,1);

        QLabel *lb8 = new QLabel("Высота основного окна", settingsform);
        grid->addWidget(lb8,7,0);
        spb5 = new QSpinBox(settingsform);
        spb5->setRange(0,qscr.availableGeometry().height());
        spb5->setValue(height);
        grid->addWidget(spb5,7,1);

        QLabel *lb9 = new QLabel("Имя файла для настроек", settingsform);
        grid->addWidget(lb9,8,0);
        led2 = new QLineEdit(settingsform);
        led2->setText(configfile);
        grid->addWidget(led2,8,1);

        QLabel *lb10 = new QLabel("Цвет основного шрифта", settingsform);
        grid->addWidget(lb10,9,0);
        fpb = new QPushButton(settingsform);
        fpb->setText("Цвет шрифта");
        QObject::connect(fpb,SIGNAL(clicked(bool)),this,SLOT(fontcolbutton()));
        grid->addWidget(fpb,9,1);

        QLabel *lb11 = new QLabel("HTML код до нужного фрагмента", settingsform);
        grid->addWidget(lb11,10,0);
        hled1 = new QLineEdit(settingsform);
        hled1->setText(HTMLbefore);
        grid->addWidget(hled1,10,1);

        QLabel *lb12 = new QLabel("HTML код после нужного фрагмента", settingsform);
        grid->addWidget(lb12,11,0);
        hled2 = new QLineEdit(settingsform);
        hled2->setText(HTMLafter);
        grid->addWidget(hled2,11,1);

        QPushButton *pbOK = new QPushButton(settingsform);
        pbOK->setText("OK");

        QObject::connect(pbOK,SIGNAL(clicked(bool)),this,SLOT(acceptNewSettings()));
        grid->addWidget(pbOK,12,0);

        QPushButton *pbCan = new QPushButton(settingsform);
        pbCan->setText("Отмена");
        QObject::connect(pbCan,SIGNAL(clicked(bool)),this,SLOT(discardSettings()));
        grid->addWidget(pbCan,12,1);

        tempcol1 = color1;
        tempcol2 = color2;

        settingsform->show();

    }

    void acceptNewSettings() {
        // сохраняем измененные пользователем настройки сначала устанавливая члены класса
        qreal opac = ((qreal)spb2->value()) / 100;
        period=spb1->value();
        url=led1->text();
        color1=tempcol1;
        color2=tempcol2;
        fontcol=tempcolfont;
        opacity=opac;
        fontsize=spb3->value();
        width=spb4->value();
        height=spb5->value();
        HTMLbefore=hled1->text();
        HTMLafter=hled2->text();
        configfile=led2->text();
        // потом применяя эти изменения на лету
        emit applysignal();
        // и наконец, сохраняем все эти настройки
        saveall();

        settingsform->hide();

    }

private slots:
    void color1button() {
        QColor tmpcolor = QColorDialog::getColor(color1);
        if (tmpcolor.isValid()) {
            tempcol1 = tmpcolor;
        }
    }

    void color2button() {
        QColor tmpcolor = QColorDialog::getColor(color2);
        if (tmpcolor.isValid()) {
            tempcol2 = tmpcolor;
        }
    }

    void fontcolbutton() {
        QColor tmpcolor = QColorDialog::getColor(fontcol);
        if (tmpcolor.isValid()) {
            tempcolfont = tmpcolor;
        }
    }

    void discardSettings() {
        settingsform->close();
    }

};

namespace Ui {
class GradusWidget;
}

class GradusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GradusWidget(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    ~GradusWidget();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *pe)
    {
        m_pmnu->exec(pe->globalPos());
    }

public slots:
    void timerexpired();
    void readdata(QUrl url);
    void showAlarm();
    void timerSetInterval(int period)
    {
        timer->setInterval(1000*period);
    }

    void changecolor(QColor color1, QColor color2)
    {
        this->setStyleSheet("background: qlineargradient(x1:0 y1:0, x2:0 y2:1 stop:0 " +
                            color1.name() + ", stop:1 " + color2.name() + ");");
    }
    void setOpacity(qreal op)
    {
        this->setWindowOpacity(op);
    }

    void setFontSize(int fsize)
    {
        myfont.setPointSize(fsize);
    }

    void setSize(int width,int height) {
        this->setGeometry(this->x(),this->y(),width,height);
    }

    void showAboutWindow()
    {
        about = new QDialog(this);
        about->setModal(true);
        about->setWindowTitle("О программе");
        about->setStyleSheet("background-color: white;");
        QGridLayout *gr = new QGridLayout(about);
        QLabel *aboutmes = new QLabel("Виджет температуры v2.0\nАвтор - Неклюдов Константин.\nE-mail: odexed@mail.ru",about);
        about->setLayout(gr);
        QPushButton *ok = new QPushButton(about);
        ok->setText("OK");
        connect(ok,SIGNAL(clicked()),about,SLOT(close()));
        gr->addWidget(aboutmes,0,0);
        gr->addWidget(ok,1,0);
        about->exec();
    }

    // применить новые настройки к форме
    void ApplyNewSettings()
    {
        timerSetInterval(cursettings->period);
        changecolor(cursettings->color1,cursettings->color2);
        setOpacity(cursettings->opacity);
        setFontSize(cursettings->fontsize);
        templabel->setStyleSheet("color: " + cursettings->fontcol.name() + ";");
        setSize(cursettings->width,cursettings->height);
        templabel->setAlignment(Qt::AlignCenter);
    }

private:
    Ui::GradusWidget *ui;
    QMenu* m_pmnu;
    QAction *quitAction;
    QAction *setAction;
    QAction *aboutAction;
    QLabel *templabel;
    QNetworkAccessManager *manager;
    QString HTMLbefore;
    QString HTMLafter;
    unsigned int period;
    QString url;
    QTimer *timer;
    QFont myfont;
    QDialog *about;
    bool isAlarm;
    int fontstyle;
    MySettings *cursettings;
private slots:
    void showTemperature(QString string);
    void replyFinished(QNetworkReply *reply);

};

#endif // GRADUSWIDGET_H
