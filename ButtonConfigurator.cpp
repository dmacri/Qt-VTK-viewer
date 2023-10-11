// buttonconfigurator.cpp
#include "ButtonConfigurator.h"
#include "qapplication.h"

void ButtonConfigurator::configureButton(QPushButton* button, QStyle::StandardPixmap icon, const QString& styleSheet)
{
    button->setIcon(QApplication::style()->standardIcon(icon));
    button->setIconSize(QSize(32, 32));
    button->setMinimumSize(QSize(100, 50));
    button->setStyleSheet(styleSheet);
}
