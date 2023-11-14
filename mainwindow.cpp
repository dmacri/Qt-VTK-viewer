#include "mainwindow.h"
#include "qcommonstyle.h"
#include "QSettings"
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
    setTotalStepsFromConfiguration(argv[1]);
}

void MainWindow::setupConnections()
{
    connectButtons();
    connectSliders();
}

void MainWindow::configureLineEdit()
{
    QIntValidator* validator = new QIntValidator(ui->lineEdit);
    ui->lineEdit->setValidator(validator);
}

void MainWindow::configureButtons()
{
    configureButton(ui->pushButton, QStyle::SP_ArrowRight, styleSheetButtonLeftColumn);
    configureButton(ui->pushButton_2, QStyle::SP_ArrowLeft, styleSheetButtonLeftColumn);
    configureButton(ui->pushButton_3, QStyle::SP_MediaPlay, styleButtonGotoStep);
    configureButton(ui->playButton, QStyle::SP_MediaSeekForward, styleSheet);
    configureButton(ui->stopButton, QStyle::SP_MediaStop, styleSheet);
    configureButton(ui->backButton, QStyle::SP_MediaSkipBackward, styleSheet);
}

void MainWindow::configureButton(QPushButton* button, QStyle::StandardPixmap icon, const QString& styleSheet)
{
    button->setIcon(style.standardIcon(icon));
    button->setIconSize(QSize(32, 32));
    button->setMinimumSize(QSize(100, 50));
    button->setStyleSheet(styleSheet);
}

void MainWindow::configureSliders()
{
    ui->sleepSlider->setMinimum(0);
    ui->sleepSlider->setMaximum(100);
    ui->sleepSlider->setValue(0);
    ui->sleepSlider->setStyleSheet(styleSheetSleep);
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
{  int stepIncrement=0;
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == ui->playButton) {
        currentStep++;
        stepIncrement=+1;
    } else if (button == ui->backButton) {
        currentStep--;
        stepIncrement=-1;
    }

    currentStep = qBound(0, currentStep, totalSteps);

    for (int step = currentStep; step < totalSteps; step += stepIncrement) {
        ui->sceneWidget->selectedStepParameter(std::to_string(step));
        QThread::msleep(sleepDuration);
        currentStep=step;
        QApplication::processEvents();
        if (!isPlaying||(isBacking && currentStep == 0)) {
            break;
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



