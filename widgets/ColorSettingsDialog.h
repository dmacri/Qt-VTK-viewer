#pragma once

#include <QDialog>
#include <QColor>

namespace Ui
{
class ColorSettingsDialog;
}

class ColorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorSettingsDialog(QWidget *parent = nullptr);
    ~ColorSettingsDialog();

private slots:
    void onBackgroundColorClicked();
    void onTextColorClicked();
    void onGridColorClicked();
    void onHighlightColorClicked();
    void onResetColors();
    void onAccepted();
    void onRejected();
    void updateColorPreviews();

protected:
    void updateColorButton(QPushButton* button, const QColor& color);
    bool selectColor(QColor& color, const QString& title);
    void loadSettings();
    void saveSettings();

private:
    Ui::ColorSettingsDialog *ui;

    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_gridColor;
    QColor m_highlightColor;
};
