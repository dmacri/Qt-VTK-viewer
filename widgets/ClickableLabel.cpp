#include <QMouseEvent>
#include "ClickableLabel.h"


ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent)
{}

void ClickableLabel::setFileName(QString fileName)
{
    this->fileName = fileName;
    setText(tr("Input file: ") + fileName);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
    }
    QLabel::mouseDoubleClickEvent(event);
}
