#include <QPixmap>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "widgets/AboutDialog.h"


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
    imageLabel(new QLabel(this)),
    textLabel(new QLabel(this))
{
    setWindowTitle("About Scimmione Configurator");

    // --- Layout ---
    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Image setup ---
    QPixmap pix(":/icons/scimmioneAtWork.png");
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

    // --- Window size & position ---
    resize(scaled.width() + 40, scaled.height() + 120);
    move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());
}
