#include "mainwindow.h"
#include "qcommonstyle.h"
#include "qthread.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QIntValidator>
#include <QPushButton>

#include <vtkDataSetReader.h>
#include <Visualizer.hpp>


MainWindow::MainWindow(QWidget* parent, int argc, char *argv[]) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setIcon(style.standardIcon(QStyle::SP_ArrowRight));
    ui->pushButton->setIconSize(QSize(32, 32));
    ui->pushButton->setMinimumSize(QSize(100, 50));
    ui->pushButton_2->setIcon(style.standardIcon(QStyle::SP_ArrowLeft));
    ui->pushButton_2->setIconSize(QSize(32, 32));
    ui->pushButton_2->setMinimumSize(QSize(100, 50));
    ui->playButton->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_3->setStyleSheet(styleSheet);
    ui->playButton->setStyleSheet(styleSheet);
    ui->playButton->setMinimumSize(QSize(300, 50));
    ui->sceneWidget->addVisualizer(argc, argv);


    connect(ui->playButton, &QPushButton::clicked, [=]() {
        QStringList listParameterFromConfiguration=  readNLinesFromFile(argv[1]);
        QString stringNumStep=listParameterFromConfiguration[7];
        QStringList step=stringNumStep.split(":");
        togglePlay(step[1].toInt());
    });

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showAboutDialog()
{

    QMessageBox::information(
                this, "About",
                "By Davide Macri.\n Configurator for  visualizer");
}

void MainWindow::showOpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                                    "VTK Files (*.vtk)");

    // Open file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
    if (!file.exists())
        return;


}

void MainWindow::on_pushButton_clicked()
{
}

void MainWindow::togglePlay(int numStep)
{
    // Verifica lo stato corrente del pulsante
    bool isPlaying = (ui->playButton->text() == "Stop");

    if (isPlaying) {
        // Se è nello stato di riproduzione, interrompi l'iterazione
        ui->playButton->setText("Play");
        return;
    }

    // Cambia lo stato del pulsante in "Stop"
    ui->playButton->setText("Stop");
    ui->playButton->setIcon(style.standardIcon(QStyle::SP_MediaStop));
    // Esegui l'iterazione
    for (int step = 1; step < numStep; ++step) {
        ui->sceneWidget->increaseCountUp();
        QThread::msleep(100);

        // Aggiornamento grafico
        QApplication::processEvents();

        // Verifica lo stato corrente del pulsante
        isPlaying = (ui->playButton->text() == "Stop");

        if (!isPlaying) {
            // Se il testo del pulsante è tornato a "Play", interrompi l'iterazione
            break;
        }
    }

    // Ripristina il testo del pulsante a "Play" alla fine dell'iterazione
    ui->playButton->setText("Play");
    ui->playButton->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->sceneWidget->decreaseCountDown();
}


void MainWindow::on_pushButton_3_clicked()
{
    QIntValidator *validator = new QIntValidator(ui->lineEdit);
    ui->lineEdit->setValidator(validator);
    string textEdited = (string) ui->lineEdit->text().toUtf8().constData();
    ui->sceneWidget->selectedStepParameter(textEdited);
}

QStringList MainWindow::readNLinesFromFile(const QString& filePath)
{
    QStringList lines;

    // Apri il file in sola lettura
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Impossibile aprire il file" << filePath;
        return lines;
    }

    // Leggi il contenuto del file riga per riga
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        lines.append(line);
    }

    // Chiudi il file
    file.close();

    return lines;
}



