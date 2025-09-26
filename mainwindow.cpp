#include <QCommonStyle>
#include <QSettings>
#include <QThread> // QThread::msleep
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <utility> // std::to_underlying, requires C++23

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/Config.h"


MainWindow::MainWindow(int argc, char* argv[], QWidget* parent) : QMainWindow(nullptr), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureUIElements(argc,argv);
    setupConnections();
}

void MainWindow::configureUIElements(int argc, char* argv[])
{
    configureButtons();
    configureSliders();
    loadStrings();
    initializeSceneWidget(argc, argv);
    showInputFilePathOnBarLabel(argv[1]);
    setTotalStepsFromConfiguration(argv[1]);

    changeWhichButtonsAreEnabled();
}

void MainWindow::setupConnections()
{
    connectMenuActions();

    connectButtons();
    connectSliders();

    connect(ui->positionSpinBox, &QSpinBox::editingFinished, this, &MainWindow::onStepNumberChanged);
    connect(ui->sceneWidget, &SceneWidget::changedStepNumberWithKeyboardKeys, ui->updatePositionSlider, &QSlider::setValue);
}

void MainWindow::connectMenuActions()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutThisApplicationDialog);
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

    const auto totalSteps = generalContext->getConfigParameter("number_steps")->getValue<int>();
    setTotalSteps(totalSteps);
}

void MainWindow::setTotalSteps(int totalStepsValue)
{
    ui->totalStep->setText(QString("/") + QString::number(totalStepsValue));
    ui->updatePositionSlider->setMaximum(totalStepsValue);
    ui->positionSpinBox->setMaximum(totalStepsValue);
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
{ // TODO: GB: Maybe we can move the file content here and make them ready for translation (surrount text with `tr(...)`)?
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

void MainWindow::showAboutThisApplicationDialog()
{
    QMessageBox::information(this, "About",
                             "By Davide Macri.\n"
                             "Configurator for visualizer");
}

void MainWindow::playingRequested(PlayingDirection direction)
{
    currentStep = std::clamp(currentStep + std::to_underlying(direction), 0, totalSteps() - 1);

    for (int step = currentStep; step >= 0 && step <= totalSteps(); step += std::to_underlying(direction))
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

        if (!isPlaying || (std::to_underlying(direction) < 0 && currentStep == 0))
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
        playingRequested(PlayingDirection::Forward);
    }
}

void MainWindow::onStopButtonClicked()
{
    isPlaying = false;
    ui->playButton->setIcon(QCommonStyle().standardIcon(QStyle::SP_MediaPlay));
}

void MainWindow::onSkipForwardButtonClicked()
{
    currentStep = totalSteps();
    setPositionOnWidgets(currentStep);
}

void MainWindow::onBackButtonClicked()
{
    isPlaying = true;
    isBacking = true;
    playingRequested(PlayingDirection::Backward);
}

void MainWindow::onLeftButtonClicked()
{
    ui->sceneWidget->decreaseCountDown();
    currentStep = std::max(currentStep - 1, 0); // TODO: GB: What is minimum acceptable step nr? is it 0 or 1?
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
    ui->positionSpinBox->setValue(stepPosition);
    ui->sceneWidget->selectedStepParameter(stepPosition);

    changeWhichButtonsAreEnabled();
}

void MainWindow::changeWhichButtonsAreEnabled()
{
    ui->rightButton->setDisabled(currentStep == totalSteps());
    ui->playButton->setDisabled(currentStep == totalSteps());
    ui->leftButton->setDisabled(currentStep <= 1);
    ui->backButton->setDisabled(currentStep <= 1);
}

void MainWindow::onStepNumberChanged()
{
    auto step = ui->positionSpinBox->value();
    if (step != currentStep)
    {
        currentStep = step;
        setPositionOnWidgets(currentStep);
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
        movingCursorSleep = true; // TODO: GB: It is set to "true" only here, should it be somewhere set to false?
    }
    cursorValueSleep = value;
    ui->sleepSlider->setToolTip("Current sleeping value " + QString::number(cursorValueSleep));
}

void MainWindow::onUpdatePositionOnSlider(int value)
{
    qDebug() << "Step is " << value;

    {
        QSignalBlocker blockSpinBox(ui->positionSpinBox);
        ui->positionSpinBox->setValue(value);
    }

    currentStep = value;

    setPositionOnWidgets(value, /*updateSlider=*/false);
}
