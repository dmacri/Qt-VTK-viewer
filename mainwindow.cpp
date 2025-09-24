#include <QCommonStyle>
#include <QSettings>
#include <QLabel>
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QIntValidator>
#include <QPushButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Config.h"

#include <vtkDataSetReader.h>
#include <Visualizer.hpp>


MainWindow::MainWindow(int argc, char* argv[], QWidget* parent) : QMainWindow(nullptr), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureUIElements(argc,argv);
    setupConnections();
}

void MainWindow::configureUIElements(int argc, char* argv[])
{
    addValidatorForPositionInputWidget();
    configureButtons();
    configureSliders();
    loadStrings();
    initializeSceneWidget(argc, argv);
    showInputFilePathOnBarLabel(argv[1]);
    setTotalStepsFromConfiguration(argv[1]);
}

void MainWindow::setupConnections()
{
    connectButtons();
    connectSliders();

    connect(ui->positionLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onStepNumberInputed);
}

void MainWindow::addValidatorForPositionInputWidget()
{
    QIntValidator* validator = new QIntValidator(ui->positionLineEdit);
    ui->positionLineEdit->setValidator(validator);
}

void MainWindow::configureButtons()
{
    configureButton(ui->rightButton, QStyle::SP_ArrowRight);
    configureButton(ui->leftButton, QStyle::SP_ArrowLeft);
    configureButton(ui->skipForwardButton, QStyle::SP_MediaSkipForward);
    configureButton(ui->playButton, QStyle::SP_MediaPlay);
    configureButton(ui->stopButton, QStyle::SP_MediaStop);
    configureButton(ui->backButton, QStyle::SP_MediaSeekBackward);
}

void MainWindow::configureButton(QPushButton* button, QStyle::StandardPixmap icon)
{
    button->setIcon(QCommonStyle().standardIcon(icon));
    button->setIconSize(QSize(32, 32));
    button->setMinimumSize(QSize(36, 32));
    button->setStyleSheet(NULL);
}

void MainWindow::configureSliders()
{
    ui->sleepSlider->setMinimum(1);
    ui->sleepSlider->setMaximum(100);
    ui->sleepSlider->setValue(50);
    // ui->sleepSlider->setStyleSheet(styleSheetSleep);
}
void MainWindow::configureCursorPosition()
{
    ui->updatePositionSlider->setMinimum(1);
    ui->updatePositionSlider->setMaximum(100);
    ui->updatePositionSlider->setValue(1);
}


void MainWindow::showInputFilePathOnBarLabel(const QString& inputFilePath)
{
    ui->inputFilePathLabel->setText(tr("Input file: ") + inputFilePath);
}

void MainWindow::initializeSceneWidget(int argc, char* argv[])
{
    ui->sceneWidget->addVisualizer(argc, argv);
}

void MainWindow::setTotalStepsFromConfiguration(char* configurationFile)
{
    Config config(configurationFile);
    config.readConfigFile();
    ConfigCategory* generalContext = config.getConfigCategory("GENERAL");

   // QStringList listParameterFromConfiguration = readNLinesFromFile(configurationFile);
   //QString stringNumStep = listParameterFromConfiguration[7];
   //QStringList step = stringNumStep.split(":");
    const auto totalSteps = (intptr_t) generalContext->getConfigParameter("number_steps")->getValue();
    setTotalSteps(totalSteps);
}

void MainWindow::setTotalSteps(int totalStepsValue)
{
    ui->totalStep->setText(QString("/") + QString::number(totalStepsValue));
    ui->updatePositionSlider->setMaximum(totalStepsValue);
}

int MainWindow::totalSteps() const
{
    return ui->updatePositionSlider->maximum();
}

void MainWindow::connectButtons()
{
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
    connect(ui->skipForwardButton, &QPushButton::clicked, this, &MainWindow::onSkipForwardButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    connect(ui->leftButton, &QPushButton::clicked, this, &MainWindow::onLeftButtonClicked);
    connect(ui->rightButton, &QPushButton::clicked, this, &MainWindow::onRightButtonClicked);
}

void MainWindow::connectSliders()
{
    connect(ui->sleepSlider, &QSlider::valueChanged, this, &MainWindow::updateSleepDuration);
    connect(ui->updatePositionSlider, &QSlider::valueChanged, this, &MainWindow::onUpdatePositionOnSlider);
}

void MainWindow::loadStrings()
{
    QString iniFilePath = QApplication::applicationDirPath() + "/app_strings.ini";
    QSettings settings(iniFilePath, QSettings::IniFormat);
    qDebug() <<"Il file path è" << settings.fileName();
    noSelectionMessage = settings.value("Messages/noSelectionWarning").toString();
    qDebug() <<"Il messaggio  è" << noSelectionMessage;

    directorySelectionMessage = settings.value("Messages/directorySelectionWarning").toString();
    compilationSuccessfulMessage = settings.value("Messages/compilationSuccessful").toString();
    compilationFailedMessage = settings.value("Messages/compilationFailed").toString();
    deleteSuccessfulMessage = settings.value("Messages/deleteSuccessful").toString();
    deleteFailedMessage = settings.value("Messages/deleteFailed").toString();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAboutDialog()
{
    QMessageBox::information(this, "About",
                             "By Davide Macri.\n"
                             "Configurator for visualizer");
}

void MainWindow::showOpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, /*caption=*/tr("Open file"), /*dir=*/"", /*filter=*/"VTK Files (*.vtk)");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "File can not be opened!", "The file '" + fileName + "' can not be opened!");
        return;
    }

    if (!file.exists())
    {
        QMessageBox::warning(this, "File does not exist!", "The file '" + fileName + "' does not exist!");
        return;
    }
    // TODO: GB: Should this function do something? Or it is to remove?
}

void MainWindow::playingRequested(int direction)
{
    currentStep = std::clamp(currentStep + direction, 0, totalSteps() - 1);

    for (int step = currentStep; step >= 0 && step <= totalSteps(); step += direction)
    {
        currentStep = step;

        {
            QSignalBlocker blockSlider(ui->updatePositionSlider);
            setPositionOnWidgets(currentStep);
        }

        if (movingCursorSleep && currentStep < totalSteps() / 2)
        {
            int sleep = totalSteps() / 2 - cursorValueSleep;
            QThread::msleep(sleep * 5);
        }

        QApplication::processEvents();

        if (!isPlaying || (direction < 0 && currentStep == 0))
        {
            break;
        }
    }
}


void MainWindow::onPlayButtonClicked()
{
    if (isPlaying)
    {
        isPlaying = false;
    }
    else
    {
        isPlaying = true;
        isBacking = false;
        playingRequested(+1);
    }
}

void MainWindow::onStopButtonClicked()
{
    isPlaying = false;
    ui->playButton->setIcon(QCommonStyle().standardIcon(QStyle::SP_MediaPlay));
}

void MainWindow::onSkipForwardButtonClicked()
{
    QMessageBox::warning(this, "Question", "What the button should do?");
}

void MainWindow::onBackButtonClicked()
{
    isPlaying = true;
    isBacking = true;
    playingRequested(-1);
}

void MainWindow::onLeftButtonClicked()
{
    ui->sceneWidget->decreaseCountDown();
    currentStep = std::max(currentStep - 1, 0);
    setPositionOnWidgets(currentStep);
}

void MainWindow::onRightButtonClicked()
{
    ui->sceneWidget->increaseCountUp();
    currentStep = std::min(currentStep + 1, totalSteps() - 1);
    setPositionOnWidgets(currentStep);
}

void MainWindow::setPositionOnWidgets(int stepPosition, bool updateSlider)
{    
    if (updateSlider)
    {
        QSignalBlocker sliderBlocker(ui->updatePositionSlider);
        ui->updatePositionSlider->setValue(stepPosition);
    }
    ui->positionLineEdit->setText(QString::number(stepPosition));
    ui->sceneWidget->selectedStepParameter(stepPosition);
}

void MainWindow::onStepNumberInputed()
{
    // TODO: GB: Why not to use SpinBox instead of LineEdit?
    QString text = ui->positionLineEdit->text();
    if (!text.isEmpty())
    {
        bool conversionOk;
        int step = text.toInt(&conversionOk);
        if (conversionOk)
        {
            ui->sceneWidget->selectedStepParameter(step);
            currentStep = step;
            setPositionOnWidgets(currentStep);
        }
        else
        {
            QMessageBox::critical(this, tr("Errore"), tr("Problemi con il numero che hai inserito"));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Errore"), tr("Inserisci un valore numerico"));
    }
}

void MainWindow::updateSleepDuration(int value)
{
    const int deltaStep = totalSteps() / 10;
    const double positionPercentage = (double) ui->sleepSlider->value() / totalSteps();
    const int sliderMaxValue = ui->sleepSlider->maximum();
    const int sliderHalve = sliderMaxValue / 2;
    const double sliderNormalizedPosition = positionPercentage * sliderMaxValue / sliderMaxValue;
    if (value > sliderHalve && value <= sliderMaxValue && isBacking == false)
    {
        stepIncrement = sliderNormalizedPosition * deltaStep;
    }
    else if(value > sliderHalve && value <= sliderMaxValue && isBacking == true)
    {
        stepIncrement = - sliderNormalizedPosition * deltaStep;
    }
    else
    {
        movingCursorSleep = true;
    }
    cursorValueSleep = value;
    ui->sleepSlider->setToolTip("Current sleeping value " + QString::number(cursorValueSleep));
}

void MainWindow::onUpdatePositionOnSlider(int value)
{
    qDebug() << "Step is " << value;

    {
        QSignalBlocker blockLineEdit(ui->positionLineEdit);
        ui->positionLineEdit->setText(QString::number(value));
    }

    currentStep = value;

    setPositionOnWidgets(value, /*updateSlider=*/false);
}

QStringList MainWindow::readNLinesFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file '" << filePath << "'";
        return {};
    }

    QStringList lines;
    for (QTextStream stream(&file); !stream.atEnd(); )
    {
        QString line = stream.readLine();
        lines.append(line);
    }

    return lines;
}
