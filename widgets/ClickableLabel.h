#pragma once

#include <QLabel>

class QMouseEvent;


class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr);

    void setFileName(QString fileName);

    const QString &getFileName() const
    {
        return fileName;
    }

signals:
    void doubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QString fileName;
};
