#include <QCommonStyle>
#include <QSettings>
#include <QThread> // QThread::msleep
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressDialog>
#include <utility> // std::to_underlying, requires C++23

#include <vtkOggTheoraWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/Config.h"
#include "widgets/ConfigDetailsDialog.h"


namespace
{
constexpr int FIRST_STEP_NUMBER = 1;
}


MainWindow::MainWindow(const QString& configFileName, QWidget* parent) : QMainWindow(nullptr), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configureUIElements(configFileName);
    setupConnections();
}

void MainWindow::configureUIElements(const QString& configFileName)
{
    configureButtons();
    loadStrings();
    initializeSceneWidget(configFileName);
    showInputFilePathOnBarLabel(configFileName);
    setTotalStepsFromConfiguration(configFileName);

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

void MainWindow::configureCursorPosition()
{
    ui->updatePositionSlider->setMinimum(FIRST_STEP_NUMBER);
    ui->updatePositionSlider->setMaximum(100);
    ui->updatePositionSlider->setValue(FIRST_STEP_NUMBER);
}


void MainWindow::showInputFilePathOnBarLabel(const QString& inputFilePath)
{
    ui->inputFilePathLabel->setFileName(inputFilePath);
}

void MainWindow::initializeSceneWidget(const QString& configFileName)
{
    ui->sceneWidget->addVisualizer(configFileName.toStdString());
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
    
    // Setup VTK video writer
    vtkNew<vtkOggTheoraWriter> writer;
    writer->SetFileName(outputFilePath.toStdString().c_str());
    writer->SetRate(fps);
    writer->SetQuality(2); // Quality 0-2, where 2 is highest
    
    // Setup window to image filter
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(ui->sceneWidget->renderWindow());
    windowToImageFilter->SetScale(1); // Image quality scale
    windowToImageFilter->SetInputBufferTypeToRGB();
    windowToImageFilter->ReadFrontBufferOff(); // Read from back buffer
    
    // Connect filter to writer
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    
    // Start writing
    writer->Start();
    
    // Iterate through all steps and capture frames
    for (int step = FIRST_STEP_NUMBER; step <= totalSteps(); ++step)
    {
        // Check if user cancelled
        if (progress.wasCanceled())
        {
            writer->End();
            throw std::runtime_error("Video export cancelled by user");
        }
        
        // Update progress
        progress.setValue(step);
        progress.setLabelText(tr("Exporting video... Step %1 of %2").arg(step).arg(totalSteps()));
        
        // Set the step and render
        currentStep = step;
        {
            QSignalBlocker blockSlider(ui->updatePositionSlider);
            setPositionOnWidgets(currentStep);
        }
        
        // Force render and process events
        ui->sceneWidget->renderWindow()->Render();
        QApplication::processEvents();
        
        // Capture frame
        windowToImageFilter->Modified();
        writer->Write();
    }
    
    // Finalize video
    writer->End();
    
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
    ui->sceneWidget->increaseCountUp();
    currentStep = std::min(currentStep + 1, totalSteps());
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
