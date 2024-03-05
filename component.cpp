#include "component.h"

Component::Component(QWidget *parent) : QWidget(parent)
{
    setupButtonUI();
    setupButtonConnections();
}

Component::~Component()
{
}

void Component::setupButtonUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    button = new QPushButton("Click me");
    layout->addWidget(button);
}

void Component::setupButtonConnections()
{
    connect(button, &QPushButton::pressed, this, &Component::onButtonPressed);
}

void Component::onButtonPressed()
{
    qDebug() << "Button pressed!";
}
