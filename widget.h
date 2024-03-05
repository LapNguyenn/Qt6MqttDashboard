#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtMqtt/QMqttClient>
#include <QtWebView>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


public slots:
    void setClientPort(int p);

private slots:
    void on_buttonConnect_clicked();
    void updateLogStateChange();
    void brokerDisconnected();
    void on_buttonSubscribe_clicked();
    void on_buttonPublish_clicked();
    void on_buttonQuit_clicked();
    void on_buttonDashboard_clicked();
    void on_buttonSetupHost_clicked();
    void on_toolButtonExpand_clicked();
    void on_pushButtonLogin_clicked();
    void loginDisplay();
    void MainPage();
    void checkLoginSuccess();
    void initClientConnection();
    void DisplayDashboardPage();
private:
    Ui::Widget *ui;
    QMqttClient *m_client;
};
#endif // WIDGET_H
