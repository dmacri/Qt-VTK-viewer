#include "widgets/AboutDialog.h"
#include <QPixmap>
#include <QGuiApplication>
#include <QScreen>


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
    imageLabel(new QLabel(this)),
    textLabel(new QLabel(this)),
    player(new QMediaPlayer(this)),
    audioOutput(nullptr)
{
    setWindowTitle("About Scimmione Configurator");

    // --- Layout ---
    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Image setup ---
    QPixmap pix(":/icons/scimmioneAtWork.png");  // <- wstaw swoją ścieżkę do pliku
    int maxWidth = 400;
    QPixmap scaled = pix.scaledToWidth(maxWidth, Qt::SmoothTransformation);
    imageLabel->setPixmap(scaled);
    imageLabel->setAlignment(Qt::AlignCenter);

    // --- Text setup ---
    textLabel->setText("<b>By Davide Macri</b><br>Configurator for Visualizer");
    textLabel->setAlignment(Qt::AlignCenter);

    // --- Close button ---
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &AboutDialog::accept);

    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    layout->addWidget(closeButton);
    setLayout(layout);

    // --- Audio setup ---
    // player->setAudioOutput(audioOutput);
    player->setMedia(QUrl("qrc:/icons/refactor.mp4"));
    player->setVolume(60);
    // player->setSource(QUrl("qrc:/icons/refactor.mp4"));  // <- Twoja piosenka w zasobach
    // audioOutput->setVolume(0.6);
    player->play();

    // --- Window size & position ---
    resize(scaled.width() + 40, scaled.height() + 120);
    move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());
}

AboutDialog::~AboutDialog()
{
    player->stop();
}
