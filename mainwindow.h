#pragma once

#include <QMainWindow>
#include <QStyle>

namespace Ui {
class MainWindow;
}

class QPushButton;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& configFileName, QWidget* parent = nullptr);
    ~MainWindow();

private slots:

    void showAboutThisApplicationDialog();
    void showConfigDetailsDialog();
    void exportVideoDialog();
    void onStepNumberChanged();
    void onOpenConfigurationRequested();

    void onModelBallSelected();
    void onModelSciddicaTSelected();
    void onReloadDataRequested();

    void onPlayButtonClicked();
    void onStopButtonClicked();
    void onSkipForwardButtonClicked();
    void onSkipBackwardButtonClicked();
    void onBackButtonClicked();
    void onLeftButtonClicked();
    void onRightButtonClicked();

    void onUpdatePositionOnSlider(int value);

private:
    enum class PlayingDirection
    {
        Forward = +1,
        Backward = -1
    };

    void playingRequested(PlayingDirection direction);

    void configureUIElements(const QString& configFileName);
    void setupConnections();
    void configureButtons();
    void configureButton(QPushButton* button, QStyle::StandardPixmap icon);
    void initializeSceneWidget(const QString& configFileName);
    void setTotalStepsFromConfiguration(const QString& configurationFile);
    void connectButtons();
    void connectSliders();
    void connectMenuActions();
    void showInputFilePathOnBarLabel(const QString& inputFilePath);

    void loadStrings();

    void setPositionOnWidgets(int stepPosition, bool updateSlider=true);

    void setTotalSteps(int totalStepsValue);
    int totalSteps() const;

    void changeWhichButtonsAreEnabled();

    void recordVideoToFile(const QString& outputFilePath, int fps);

    Ui::MainWindow* ui;

    int currentStep;
    bool isPlaying = false;
    bool isBacking = false;

    QString noSelectionMessage;
    QString directorySelectionMessage;
    QString compilationSuccessfulMessage;
    QString compilationFailedMessage;
    QString deleteSuccessfulMessage;
    QString deleteFailedMessage;
};
