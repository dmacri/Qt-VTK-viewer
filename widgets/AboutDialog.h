#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    QLabel *imageLabel;
    QLabel *textLabel;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
};

#endif // ABOUTDIALOG_H
