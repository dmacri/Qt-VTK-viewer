#include "mainwindow.h"
#include "qcommonstyle.h"
#include "QSettings"
#include "qlabel.h"
#include "qthread.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QIntValidator>
#include <QPushButton>

#include <vtkDataSetReader.h>
#include <Visualizer.hpp>


MainWindow::MainWindow(QWidget* parent, int argc, char* argv[]) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureUIElements(argc,argv);
    setupConnections();
}

void MainWindow::configureUIElements(int argc, char* argv[])
{
    configureLineEdit();
    configureButtons();
    configureSliders();
    loadStrings();
    initializeSceneWidget(argc, argv);
    configureStatusBarLabel(argv[1]);
    setTotalStepsFromConfiguration(argv[1]);
}

void MainWindow::setupConnections()
{
    connectButtons();
    connectSliders();
    connectSliderForPosition();
}

void MainWindow::configureLineEdit()
{
    QIntValidator* validator = new QIntValidator(ui->lineEdit);
    ui->lineEdit->setValidator(validator);
}

void MainWindow::configureButtons()
{
    configureButton(ui->pushButton, QStyle::SP_ArrowRight, NULL);
    configureButton(ui->pushButton_2, QStyle::SP_ArrowLeft, NULL);
    configureButton(ui->pushButton_3, QStyle::SP_MediaSkipForward, NULL);
    configureButton(ui->playButton, QStyle::SP_MediaPlay, NULL);
    configureButton(ui->stopButton, QStyle::SP_MediaStop, NULL);
    configureButton(ui->backButton, QStyle::SP_MediaSeekBackward, NULL);
}

void MainWindow::configureButton(QPushButton* button, QStyle::StandardPixmap icon, const QString& styleSheet)
{
    button->setIcon(style.standardIcon(icon));
    button->setIconSize(QSize(32, 32));
    button->setMinimumSize(QSize(36, 32));
    button->setStyleSheet(NULL);
}

void MainWindow::configureSliders()
{
    ui->sleepSlider->setMinimum(1);
    ui->sleepSlider->setMaximum(100);
    ui->sleepSlider->setValue(0);
    // ui->sleepSlider->setStyleSheet(styleSheetSleep);

}
void MainWindow::configureCursorPosition()
{
    ui->updatePositionSlider->setMinimum(1);
    ui->updatePositionSlider->setMaximum(100);
    ui->updatePositionSlider->setValue(0);
}


void MainWindow::configureStatusBarLabel(const QString& inputFilePath)
{
    const QString& inputfile="Input file: ";
    QLabel *label = new QLabel(inputfile+inputFilePath);
    ui->statusbar->addWidget(label);
}

void MainWindow::initializeSceneWidget(int argc, char* argv[])
{
    ui->sceneWidget->addVisualizer(argc, argv);
}

void MainWindow::setTotalStepsFromConfiguration(const char* configurationFile)
{
    QStringList listParameterFromConfiguration = readNLinesFromFile(configurationFile);
    QString stringNumStep = listParameterFromConfiguration[7];
    QStringList step = stringNumStep.split(":");
    totalSteps = step[1].toInt();
}

void MainWindow::connectButtons()
{
    connectButton(ui->playButton);
    connectButton(ui->backButton);
    connectButton(ui->stopButton);
}

void MainWindow::connectButton(QPushButton* button)
{
    connect(button, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
}

void MainWindow::connectSliders()
{
    connect(ui->sleepSlider, &QSlider::valueChanged, this, &MainWindow::updateSleepDuration);
}

void MainWindow::connectSliderForPosition()
{
    connect(ui->updatePositionSlider, &QSlider::valueChanged, this, &MainWindow::updatePosition);
}

void MainWindow::loadStrings() {
    QString iniFilePath = QApplication::applicationDirPath() + "/app_strings.ini";
    QSettings settings(iniFilePath, QSettings::IniFormat);
    qDebug() <<"Il file path è" <<settings.fileName();

    noSelectionMessage = settings.value("Messages/noSelectionWarning").toString();
    qDebug() <<"Il messaggio  è" <<  noSelectionMessage;

    directorySelectionMessage = settings.value("Messages/directorySelectionWarning").toString();
    compilationSuccessfulMessage = settings.value("Messages/compilationSuccessful").toString();
    compilationFailedMessage = settings.value("Messages/compilationFailed").toString();
    deleteSuccessfulMessage = settings.value("Messages/deleteSuccessful").toString();
    deleteFailedMessage = settings.value("Messages/deleteFailed").toString();
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

    QFile file(fileName);

    file.open(QIODevice::ReadOnly);

    if (!file.exists())
        return;


}

void MainWindow::on_pushButton_clicked()
{
}


void MainWindow::togglePlay()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == ui->playButton) {
        currentStep++;
    } else if (button == ui->backButton) {
        currentStep--;
    }

    currentStep = qBound(0, currentStep, totalSteps - 1);


    int stepIncrement = (button == ui->playButton) ? 1 : -1;

    for (int step = currentStep; step >= 0 && step < totalSteps; step += stepIncrement) {
        ui->sceneWidget->selectedStepParameter(std::to_string(step));
        currentStep = step;
        double positionPercentage = (double)currentStep / totalSteps; // Calcola la percentuale di avanzamento
        int sliderMaxValue = ui->updatePositionSlider->maximum(); // Ottieni il valore massimo del cursore
        int sliderValue = positionPercentage * sliderMaxValue; // Calcola il valore del cursore basato sulla percentuale
        ui->updatePositionSlider->setValue(sliderValue); // Imposta il valore del cursore

        if (movingCursorSleep) {
            QThread::msleep(cursorValueSleep * 10);
            //movingCursorSleep=false;
        }
        updateValueAndPositionWithStep = false;
        QApplication::processEvents();
        if (!isPlaying || (isBacking && currentStep == 0)) {
            break;
        }

        if (step == totalSteps - 1) {
            updateValueAndPositionWithStep = true;
        }
    }
}



void MainWindow::handleButtonClick()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());


    if (button == ui->playButton) {
        if (ui->playButton->text() == "Stop" && isPlaying) {
            isPlaying = false;
        } else {
            isPlaying = true;
            isBacking = false;
            togglePlay();
        }
    } else if (button == ui->backButton) {
        isPlaying = true;
        isBacking = true;
        togglePlay();
    } else if (button == ui->stopButton) {
        isPlaying = false;
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
    cursorValueSleep=value;
    movingCursorSleep=true;
}

void MainWindow::updatePosition(int value)
{
    cursorValuePosition = value;
    if(updateValueAndPositionWithStep){
        movingCursorPosition=true;
        ui->sceneWidget->selectedStepParameter(std::to_string(static_cast<int>(totalSteps * (value / 100.0))));
    }
}



QStringList MainWindow::readNLinesFromFile(const QString& filePath)
{
    QStringList lines;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Impossibile aprire il file" << filePath;
        return lines;
    }

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        lines.append(line);
    }

    file.close();

    return lines;
}



