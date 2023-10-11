// buttonconfigurator.h
#ifndef BUTTONCONFIGURATOR_H
#define BUTTONCONFIGURATOR_H

#include <QPushButton>
#include <QStyle>

class ButtonConfigurator
{
public:
    static void configureButton(QPushButton* button, QStyle::StandardPixmap icon, const QString& styleSheet);
};

#endif // BUTTONCONFIGURATOR_H
