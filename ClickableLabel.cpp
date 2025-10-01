#include <QMouseEvent>
#include "ClickableLabel.h"


ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent)
{}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
    }
    QLabel::mouseDoubleClickEvent(event);
}
