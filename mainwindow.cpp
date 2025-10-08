#include <utility> // std::to_underlying, which requires C++23
#include <QCommonStyle>
#include <QSettings>
#include <QThread> // QThread::msleep
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/Config.h"
#include "widgets/ConfigDetailsDialog.h"
#include "visualiser/VideoExporter.h"


namespace
{
constexpr int FIRST_STEP_NUMBER = 0;
}


MainWindow::MainWindow(const QString& configFileName, QWidget* parent) 
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , currentStep{FIRST_STEP_NUMBER}
    , hasConfiguration{false}
{
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());
    
    setupConnections();
    configureButtons();
    loadStrings();
    
    if (configFileName.isEmpty())
    {
        // Start in "no configuration" mode
        enterNoConfigurationMode();
    }
    else
    {
        // Load configuration normally
        configureUIElements(configFileName);
    }
}

void MainWindow::configureUIElements(const QString& configFileName)
{
    initializeSceneWidget(configFileName);
    showInputFilePathOnBarLabel(configFileName);
    setTotalStepsFromConfiguration(configFileName);
    
    hasConfiguration = true;
    setWidgetsEnabledState(true);
    changeWhichButtonsAreEnabled();
}

void MainWindow::setupConnections()
{
    connectMenuActions();

    connectButtons();
    connectSliders();

    connect(ui->positionSpinBox, &QSpinBox::editingFinished, this, &MainWindow::onStepNumberChanged);
    connect(ui->sceneWidget, &SceneWidget::changedStepNumberWithKeyboardKeys, ui->updatePositionSlider, &QSlider::setValue);
    connect(ui->inputFilePathLabel, &ClickableLabel::doubleClicked, this, &MainWindow::showConfigDetailsDialog);
}

void MainWindow::connectMenuActions()
{
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutThisApplicationDialog);
    connect(ui->actionShow_config_details, &QAction::triggered, this, &MainWindow::showConfigDetailsDialog);
    connect(ui->actionExport_Video, &QAction::triggered, this, &MainWindow::exportVideoDialog);
    connect(ui->actionOpenConfiguration, &QAction::triggered, this, &MainWindow::onOpenConfigurationRequested);
    
    // Model selection actions
    connect(ui->actionModelBall, &QAction::triggered, this, &MainWindow::onModelBallSelected);
    connect(ui->actionModelSciddicaT, &QAction::triggered, this, &MainWindow::onModelSciddicaTSelected);
    connect(ui->actionReloadData, &QAction::triggered, this, &MainWindow::onReloadDataRequested);
}

void MainWindow::configureButtons()
{
    configureButton(ui->rightButton, QStyle::SP_ArrowRight);
    configureButton(ui->leftButton, QStyle::SP_ArrowLeft);
    configureButton(ui->skipForwardButton, QStyle::SP_MediaSkipForward);
    configureButton(ui->skipBackwardButton, QStyle::SP_MediaSkipBackward);
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


void MainWindow::showInputFilePathOnBarLabel(const QString& inputFilePath)
{
    ui->inputFilePathLabel->setFileName(inputFilePath);
}

void MainWindow::initializeSceneWidget(const QString& configFileName)
{
    ui->sceneWidget->addVisualizer(configFileName.toStdString(), currentStep);
}

void MainWindow::setTotalStepsFromConfiguration(const QString &configurationFile)
{
    const auto configFilePath = configurationFile.toStdString();
    Config config(configFilePath);

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
    connect(ui->skipBackwardButton, &QPushButton::clicked, this, &MainWindow::onSkipBackwardButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    connect(ui->leftButton, &QPushButton::clicked, this, &MainWindow::onLeftButtonClicked);
    connect(ui->rightButton, &QPushButton::clicked, this, &MainWindow::onRightButtonClicked);
}

void MainWindow::connectSliders()
{
    connect(ui->updatePositionSlider, &QSlider::valueChanged, this, &MainWindow::onUpdatePositionOnSlider);
}

void MainWindow::loadStrings()
{
    noSelectionMessage = tr("Nessun elemento selezionato!");
    directorySelectionMessage = tr("Nessun elemento selezionato!");
    compilationSuccessfulMessage = tr("Compilation successful.");
    compilationFailedMessage = tr("Compilation failed.");
    deleteSuccessfulMessage = tr("Delete SceneWidgetVisualizerProxy.h successful.");
    deleteFailedMessage = tr("Delete SceneWidgetVisualizerProxy.h failed.");
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

void MainWindow::showConfigDetailsDialog()
{
    const auto configFileName = ui->inputFilePathLabel->getFileName();
    if (configFileName.isEmpty())
    {
        QMessageBox::warning(this, tr("No Configuration"),
                           tr("No configuration file has been loaded."));
        return;
    }

    ConfigDetailsDialog dialog(configFileName.toStdString(), this);
    dialog.exec();
}

void MainWindow::exportVideoDialog()
{
    QString outputFilePath = QFileDialog::getSaveFileName(this,
                                                          tr("Export Video"),
                                                          /*dir=*/QString(),
                                                          tr("OGG Video Files (*.ogv);;All Files (*)"));
    
    if (outputFilePath.isEmpty())
    {
        return; // User cancelled
    }
    
    // Ensure .ogv extension
    if (! outputFilePath.endsWith(".ogv", Qt::CaseInsensitive))
    {
        outputFilePath += ".ogv";
    }
    
    const int fps = ui->speedSpinBox->value();
    
    // Record video
    try
    {
        recordVideoToFile(outputFilePath, fps);
        QMessageBox::information(this, tr("Export Complete"),
                               tr("Video exported successfully to:\n%1").arg(outputFilePath));
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Export Failed"),
                            tr("Failed to export video:\n%1").arg(e.what()));
    }
}

void MainWindow::recordVideoToFile(const QString& outputFilePath, int fps)
{
    // Save current state
    const int originalStep = currentStep;
    const bool wasPlaying = isPlaying;
    isPlaying = false;
    
    // Create progress dialog
    QProgressDialog progress(tr("Exporting video..."), tr("Cancel"), 1, totalSteps(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    
    // Create video exporter
    VideoExporter exporter;
    
    // Define callback to update visualization for each step
    auto updateStepCallback = [this](int step) {
        currentStep = step;
        QSignalBlocker blockSlider(ui->updatePositionSlider);
        setPositionOnWidgets(currentStep);
        QApplication::processEvents();
    };
    
    // Define callback to report progress
    auto progressCallback = [&progress, this](int step, int total) {
        progress.setValue(step);
        progress.setLabelText(tr("Exporting video... Step %1 of %2").arg(step).arg(total));
    };
    
    // Define callback to check if cancelled
    auto cancelledCallback = [&progress]() -> bool {
        return progress.wasCanceled();
    };
    
    // Export video using VideoExporter
    exporter.exportVideo(
        ui->sceneWidget->renderWindow(),
        outputFilePath,
        fps,
        totalSteps(),
        updateStepCallback,
        progressCallback,
        cancelledCallback
    );
    
    // Restore original state
    currentStep = originalStep;
    setPositionOnWidgets(currentStep);
    isPlaying = wasPlaying;
    
    progress.setValue(totalSteps());
}

void MainWindow::playingRequested(PlayingDirection direction)
{
    currentStep = std::clamp(currentStep + std::to_underlying(direction), FIRST_STEP_NUMBER, totalSteps());

    while (true)
    {
        currentStep = std::clamp(currentStep, FIRST_STEP_NUMBER, totalSteps());;

        {
            QSignalBlocker blockSlider(ui->updatePositionSlider);
            setPositionOnWidgets(currentStep);
        }

        QThread::msleep(ui->sleepSpinBox->value());

        QApplication::processEvents();

        if (!isPlaying || (std::to_underlying(direction) < 0 && currentStep == 0))
        {
            break;
        }

        if (PlayingDirection::Forward == direction && currentStep == totalSteps()
            || PlayingDirection::Backward == direction && currentStep == FIRST_STEP_NUMBER)
        {
            break;
        }

        currentStep += std::to_underlying(direction)*ui->speedSpinBox->value();
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

void MainWindow::onSkipBackwardButtonClicked()
{
    currentStep = FIRST_STEP_NUMBER;
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
    currentStep = std::max(currentStep - 1, FIRST_STEP_NUMBER);
    setPositionOnWidgets(currentStep);
}

void MainWindow::onRightButtonClicked()
{
    try
    {
        ui->sceneWidget->increaseCountUp();
        currentStep = std::min(currentStep + 1, totalSteps());
        setPositionOnWidgets(currentStep);
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Changing position error",
                             tr("It was impossible to change position to %1, because:\n").arg(currentStep) + e.what());
    }
}

void MainWindow::setPositionOnWidgets(int stepPosition, bool updateSlider)
{
    try
    {
        if (updateSlider)
        {
            QSignalBlocker sliderBlocker(ui->updatePositionSlider);
            ui->updatePositionSlider->setValue(stepPosition);
        }
        ui->positionSpinBox->setValue(stepPosition);
        ui->sceneWidget->selectedStepParameter(stepPosition);
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Changing position error",
                             tr("It was impossible to change position to %1, because:\n").arg(stepPosition) + e.what());
    }
    changeWhichButtonsAreEnabled();
}

void MainWindow::changeWhichButtonsAreEnabled()
{
    ui->rightButton->setDisabled(currentStep == totalSteps());
    ui->playButton->setDisabled(currentStep == totalSteps());
    ui->leftButton->setDisabled(currentStep <= FIRST_STEP_NUMBER);
    ui->backButton->setDisabled(currentStep <= FIRST_STEP_NUMBER);
    ui->skipBackwardButton->setDisabled(currentStep <= FIRST_STEP_NUMBER);
    ui->skipForwardButton->setDisabled(currentStep == totalSteps());
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

void MainWindow::onModelBallSelected()
{
    try
    {
        ui->sceneWidget->switchModel(ModelType::Ball);
        
        // Update menu checkboxes
        ui->actionModelBall->setChecked(true);
        ui->actionModelSciddicaT->setChecked(false);
        
        QMessageBox::information(this, tr("Model Changed"),
                               tr("Successfully switched to Ball model.\nUse 'Reload Data' (F5) to load data files."));
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Model Switch Failed"),
                            tr("Failed to switch model:\n%1").arg(e.what()));
        
        // Revert checkbox state
        const auto currentModel = ui->sceneWidget->getCurrentModelName();
        ui->actionModelBall->setChecked(currentModel == "Ball");
        ui->actionModelSciddicaT->setChecked(currentModel == "SciddicaT");
    }
}

void MainWindow::onModelSciddicaTSelected()
{
    try
    {
        ui->sceneWidget->switchModel(ModelType::SciddicaT);
        
        // Update menu checkboxes
        ui->actionModelBall->setChecked(false);
        ui->actionModelSciddicaT->setChecked(true);
        
        QMessageBox::information(this, tr("Model Changed"),
                               tr("Successfully switched to SciddicaT model.\nUse 'Reload Data' (F5) to load data files."));
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Model Switch Failed"),
                            tr("Failed to switch model:\n%1").arg(e.what()));
        
        // Revert checkbox state
        const auto currentModel = ui->sceneWidget->getCurrentModelName();
        ui->actionModelBall->setChecked(currentModel == "Ball");
        ui->actionModelSciddicaT->setChecked(currentModel == "SciddicaT");
    }
}

void MainWindow::onReloadDataRequested()
{
    try
    {
        ui->sceneWidget->reloadData();
        
        QMessageBox::information(this, tr("Data Reloaded"),
                               tr("Data files successfully reloaded for model: %1")
                               .arg(QString::fromStdString(ui->sceneWidget->getCurrentModelName())));
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Reload Failed"),
                            tr("Failed to reload data:\n%1").arg(e.what()));
    }
}

void MainWindow::onOpenConfigurationRequested()
{
    // Open file dialog to select configuration file
    QString configFileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Configuration File"),
        QString(), // Start in current directory
        tr("Configuration Files (*.txt *.ini);;All Files (*)")
    );
    
    if (configFileName.isEmpty())
    {
        return; // User cancelled
    }
    
    try
    {
        // Stop any ongoing playback
        isPlaying = false;
        isBacking = false;
        
        if (!hasConfiguration)
        {
            // First time loading configuration
            initializeSceneWidget(configFileName);
            hasConfiguration = true;
        }
        else
        {
            // Reload with new configuration
            ui->sceneWidget->loadNewConfiguration(configFileName.toStdString(), 0);
        }
        
        // Update UI with new configuration
        showInputFilePathOnBarLabel(configFileName);
        setTotalStepsFromConfiguration(configFileName);
        
        // Reset to first step
        currentStep = 0;
        setPositionOnWidgets(currentStep);
        
        // Enable all widgets now that we have configuration
        setWidgetsEnabledState(true);
        
        QMessageBox::information(this, tr("Configuration Loaded"),
                               tr("Successfully loaded configuration:\n%1").arg(configFileName));
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Load Failed"),
                            tr("Failed to load configuration:\n%1").arg(e.what()));
    }
}

void MainWindow::enterNoConfigurationMode()
{    
    // Set UI to show no configuration loaded
    ui->inputFilePathLabel->setFileName("");
    ui->inputFilePathLabel->setText(tr("No configuration loaded - use File → Open Configuration"));
    
    setTotalSteps(0);
    currentStep = 0;
    ui->positionSpinBox->setValue(0);
    ui->updatePositionSlider->setValue(0);
    
    // Disable all playback and navigation widgets
    setWidgetsEnabledState(false);
}

void MainWindow::setWidgetsEnabledState(bool enabled)
{
    // Playback controls
    ui->playButton->setEnabled(enabled);
    ui->stopButton->setEnabled(enabled);
    ui->backButton->setEnabled(enabled);
    ui->leftButton->setEnabled(enabled);
    ui->rightButton->setEnabled(enabled);
    ui->skipForwardButton->setEnabled(enabled);
    ui->skipBackwardButton->setEnabled(enabled);
    
    // Position controls
    ui->updatePositionSlider->setEnabled(enabled);
    ui->positionSpinBox->setEnabled(enabled);
    
    // Speed/sleep controls (if they exist)
    if (ui->speedSpinBox)
        ui->speedSpinBox->setEnabled(enabled);
    if (ui->sleepSpinBox)
        ui->sleepSpinBox->setEnabled(enabled);
    
    // Menu actions - some should remain enabled
    // actionQuit - always enabled
    // actionAbout - always enabled
    // actionOpenConfiguration - always enabled
    // actionModelBall - always enabled (can switch before loading config)
    // actionModelSciddicaT - always enabled (can switch before loading config)
    
    // These should be disabled without configuration:
    ui->actionShow_config_details->setEnabled(enabled);
    ui->actionExport_Video->setEnabled(enabled);
    ui->actionReloadData->setEnabled(enabled);
}
