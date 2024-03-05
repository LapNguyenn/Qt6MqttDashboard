#include "widget.h"
#include "./ui_widget.h"
#include "component.h"

#include <QtCore/QDateTime>
#include <QtWidgets/QMessageBox>
#include <QVBoxLayout>
#include <QSettings>
#include <QTimer>

#define DASHBOARD_STACK_INDEX 1
#define EDIT_HOST_STACK_INDEX 0
#define LOGIN_STACK_INDEX 0
#define MAINPAGE_STACK_INDEX 1

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    initClientConnection();
    loginDisplay();
}

Widget::~Widget()
{
    delete ui;
}

//Main page
void Widget::MainPage(){
    ui->stackedWidget_1->setCurrentIndex(MAINPAGE_STACK_INDEX);
    ui->stackedWidget_2->setCurrentIndex(DASHBOARD_STACK_INDEX);
}

//Dashboard Page
void Widget::DisplayDashboardPage() {
    // Tạo một widget để chứa các thành phần của trang
    QWidget *pageDisplayWidget = new QWidget();

    // Thêm widget mới vào trang hiện tại của QStackedWidget
    ui->stackedWidgetDashboard->addWidget(pageDisplayWidget);

    // Tạo một QVBoxLayout để chứa các thành phần của trang
    QVBoxLayout *layout = new QVBoxLayout(pageDisplayWidget);

    // Tạo một đối tượng mới của lớp Component và thêm nó vào layout của widget trang
    Component *button1 = new Component(pageDisplayWidget);
    layout->addWidget(button1);

    // Hiển thị nút trên trang
    button1->show();
}



//Mqtt Client
void Widget::initClientConnection(){
    m_client->setHostname(ui->lineEditHost->text());
    m_client->setPort(static_cast<quint16>(ui->spinBoxPort->value()));

    connect(m_client, &QMqttClient::stateChanged, this, &Widget::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &Widget::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                                + QLatin1String(" Received Topic: ")
                                + topic.name()
                                + QLatin1String(" Message: ")
                                + message
                                + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                                + QLatin1String(" PingResponse")
                                + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });
    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &Widget::setClientPort);
    connect(ui->lineEditUsername, &QLineEdit::textChanged, m_client, &QMqttClient::setUsername);
    connect(ui->lineEditPassword, &QLineEdit::textChanged, m_client, &QMqttClient::setPassword);
    updateLogStateChange();
}

void Widget::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                            + QLatin1String(": State Change")
                            + QString::number(m_client->state())
                            + QLatin1Char('\n');
    ui->editLog->insertPlainText(content);
}

void Widget::brokerDisconnected()
{
    ui->lineEditHost->setEnabled(true);
    ui->spinBoxPort->setEnabled(true);
    ui->buttonConnect->setText(tr("Connect"));
}

void Widget::setClientPort(int p)
{
    m_client->setPort(static_cast<quint16>(p));
}

//Stack bar
void Widget::on_buttonQuit_clicked()
{
    QApplication::quit();
}

void Widget::on_buttonDashboard_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(DASHBOARD_STACK_INDEX);
}

void Widget::on_buttonSetupHost_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(EDIT_HOST_STACK_INDEX);
}
//Setup host function
void Widget::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
        ui->lineEditUsername->setEnabled(false);
        ui->lineEditPassword->setEnabled(false);
        ui->buttonConnect->setText(tr("Disconnect"));
        m_client->connectToHost();
    } else {
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPort->setEnabled(true);
        ui->lineEditUsername->setEnabled(true);
        ui->lineEditPassword->setEnabled(true);
        ui->buttonConnect->setText(tr("Connect"));
        m_client->disconnectFromHost();
    }
}

void Widget::on_buttonSubscribe_clicked()
{
    auto subscription = m_client->subscribe(ui->lineEditTopic->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
}

void Widget::on_buttonPublish_clicked()
{
    if (m_client->publish(ui->lineEditTopic->text(), ui->lineEditMessage->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

//Login function
void Widget::on_toolButtonExpand_clicked()
{
    if(ui->groupBox_edit_host_expand->isHidden() && !ui->label_edit_host_expand->isHidden()){
        ui->groupBox_edit_host_expand->show();
        ui->label_edit_host_expand->hide();
    } else {
        ui->groupBox_edit_host_expand->hide();
        ui->label_edit_host_expand->show();
    }
}

void Widget::on_pushButtonLogin_clicked()
{
    // Check if any field is not filled
    if (ui->lineEditUserLogin->text().isEmpty() || ui->lineEditPasswordLogin->text().isEmpty() || ui->lineEditHost_2->text().isEmpty() || ui->lineEditPort_2->text().isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "Please fill all field!");
        return;
    }

    QSettings settings;
    if (ui->checkBox_remember_me->isChecked()) {
        settings.setValue("HOST", ui->lineEditHost_2->text());
        settings.setValue("PORT", ui->lineEditPort_2->text());
        settings.setValue("CHECK", ui->checkBox_remember_me->isChecked());
    } else {
        settings.remove("HOST");
        settings.remove("PORT");
        settings.remove("CHECK");
    }
    ui->lineEditHost->setText(ui->lineEditHost_2->text());
    ui->spinBoxPort->setValue(ui->lineEditPort_2->text().toInt());
    ui->lineEditUsername->setText(ui->lineEditUserLogin->text());
    ui->lineEditPassword->setText(ui->lineEditPasswordLogin->text());

    ui->pushButtonLogin->setEnabled(false);
    ui->lineEditUserLogin->setEnabled(false);
    ui->lineEditPasswordLogin->setEnabled(false);
    ui->lineEditHost_2->setEnabled(false);
    ui->lineEditPort_2->setEnabled(false);

    m_client->connectToHost();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::checkLoginSuccess);
    timer->start(1000);
    timer->singleShot(5000, [=](){
        if(m_client->state() != QMqttClient::Connected){
            QMessageBox::warning(nullptr, "Error", "Timeout: Connection Timeout.\n" + ui->editLog->toPlainText());
            ui->pushButtonLogin->setEnabled(true);
            ui->lineEditUserLogin->setEnabled(true);
            ui->lineEditPasswordLogin->setEnabled(true);
            ui->lineEditHost_2->setEnabled(true);
            ui->lineEditPort_2->setEnabled(true);
        }
    });
}

void Widget::checkLoginSuccess(){
    if(m_client->state() == QMqttClient::Connected){
        ui->stackedWidget_1->setCurrentIndex(1);
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
        ui->lineEditUsername->setEnabled(false);
        ui->lineEditPassword->setEnabled(false);
        ui->buttonConnect->setText("Disconnect");
        for(auto &timer : this->findChildren<QTimer*>()) {
            timer->stop();
            timer->deleteLater();
        }
        MainPage();
    }
}

void Widget::loginDisplay(){
    ui->stackedWidget_1->setCurrentIndex(LOGIN_STACK_INDEX);
    ui->groupBox_edit_host_expand->hide();
    ui->groupBox_2->hide();

    QSettings settings;
    ui->lineEditHost_2->setText(settings.value("HOST").toString());
    ui->lineEditPort_2->setText(settings.value("PORT").toString());
    ui->lineEditUserLogin->setText(settings.value("USERNAME").toString());
    if(settings.contains("CHECK")){
        ui->checkBox_remember_me->setChecked(true);
    }
}
