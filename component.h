#ifndef COMPONENT_H
#define COMPONENT_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>

class Component : public QWidget
{
    Q_OBJECT

public:
    Component(QWidget *parent = nullptr);
    ~Component();

private:
    QPushButton *button;
    QLabel *label;

    void setupButtonUI();
    void setupButtonConnections();

public slots:
    void onButtonPressed();

};

#endif // COMPONENT_H
