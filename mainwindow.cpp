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
    QIntValidator *validator = new QIntValidator(ui->lineEdit);
    ui->lineEdit->setValidator(validator);
    ui->pushButton->setIcon(style.standardIcon(QStyle::SP_ArrowRight));
    ui->pushButton->setStyleSheet(styleSheetButtonLeftColumn);
    ui->pushButton->setIconSize(QSize(32, 32));
    ui->pushButton->setMinimumSize(QSize(100, 50));
    ui->pushButton_2->setIcon(style.standardIcon(QStyle::SP_ArrowLeft));
    ui->pushButton_2->setIconSize(QSize(32, 32));
    ui->pushButton_2->setMinimumSize(QSize(100, 50));
    ui->pushButton_2->setStyleSheet(styleSheetButtonLeftColumn);
    ui->pushButton_3->setStyleSheet(styleButtonGotoStep);
    ui->playButton->setIcon(style.standardIcon(QStyle::SP_MediaSeekForward));
    ui->playButton->setStyleSheet(styleSheet);
    ui->playButton->setMinimumSize(QSize(200, 50));
    ui->stopButton->setIcon(style.standardIcon(QStyle::SP_MediaStop));
    ui->stopButton->setObjectName("stopButton");
    ui->stopButton->setMinimumSize(QSize(200, 50));
    ui->stopButton->setStyleSheet(styleSheet);
    ui->backButton->setMinimumSize(QSize(200, 50));
    ui->backButton->setIcon(style.standardIcon(QStyle::SP_MediaSkipBackward));
    ui->backButton->setStyleSheet(styleSheet);
    ui->sleepSlider->setMinimum(0);
    ui->sleepSlider->setMaximum(100);
    ui->sleepSlider->setValue(50);
    ui->sleepSlider->setStyleSheet(styleSheetSleep);




    ui->sceneWidget->addVisualizer(argc, argv);
    QStringList listParameterFromConfiguration = readNLinesFromFile(argv[1]);
    QString stringNumStep = listParameterFromConfiguration[7];
    QStringList step = stringNumStep.split(":");
    totalSteps = step[1].toInt();

    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::handleButtonClick);

    connect(ui->sleepSlider, &QSlider::valueChanged, this, &MainWindow::updateSleepDuration);


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

void MainWindow::togglePlay()
{  int stepIncrement=0;
    // Verifica quale tasto è stato premuto
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == ui->playButton) {
        // Tasto "Play" premuto
        // Incrementa lo step corrente
        currentStep++;
        stepIncrement=+1;
    } else if (button == ui->backButton) {
        // Tasto "Back" premuto
        // Decrementa lo step corrente
        currentStep--;
        stepIncrement=-1;
    }

    // Assicurati che currentStep sia compreso tra 0 e totalSteps
    currentStep = qBound(0, currentStep, totalSteps);

    // Esegui l'iterazione
    for (int step = currentStep; step < totalSteps; step += stepIncrement) {
        ui->sceneWidget->selectedStepParameter(std::to_string(step));
        QThread::msleep(sleepDuration);
        currentStep=step;
        // Aggiornamento grafico
        QApplication::processEvents();


        if (!isPlaying||(isBacking && currentStep == 0)) {
            // Se il testo del pulsante "Play" è stato premuto, interrompi l'iterazione
            break;
        }
    }


}
void MainWindow::handleButtonClick()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    // Verifica quale tasto è stato premuto
    if (button == ui->playButton) {
        // Tasto "Play" premuto
        if (ui->playButton->text() == "Stop" && isPlaying) {
            // Il tasto "Stop" è stato premuto
            // Interrompi l'iterazione
            isPlaying = false;
        } else {
            // Il tasto "Play" è stato premuto
            // Esegui l'iterazione in avanti
            isPlaying = true;
            isBacking = false;  // Resetta lo stato di "Back"
            togglePlay();
        }
    } else if (button == ui->backButton) {
        // Tasto "Back" premuto
        // Esegui l'iterazione all'indietro
        isPlaying = true;
        isBacking = true;   // Imposta lo stato di "Back"
        togglePlay();
    } else if (button == ui->stopButton) {
        // Tasto "Stop" premuto
        isPlaying = false;  // Interrompi l'iterazione
        // Aggiorna l'aspetto del pulsante "Play"
        ui->playButton->setText("Play");
        ui->playButton->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
    }
}




void MainWindow::on_pushButton_2_clicked()
{
    ui->sceneWidget->decreaseCountDown();
}


void MainWindow::on_pushButton_3_clicked()
{
    QString text = ui->lineEdit->text();
    if (!text.isEmpty()) {
        bool conversionOk;
        int step = text.toInt(&conversionOk);
        if (conversionOk) {
            ui->sceneWidget->selectedStepParameter(std::to_string(step));
            currentStep = step;
        } else {
            QMessageBox::critical(this, tr("Errore"), tr("Problemi con il numero che hai inserito"));        }
    } else {
   QMessageBox::critical(this, tr("Errore"), tr("Inserisci un valore numerico"));      }
}
void MainWindow::updateSleepDuration(int value)
{
    sleepDuration = value;

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



