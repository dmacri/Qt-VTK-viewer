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
    explicit MainWindow(int argc, char* argv[], QWidget* parent = nullptr);
    ~MainWindow();

private slots:

    void showAboutThisApplicationDialog();
    void onStepNumberChanged();

    void onPlayButtonClicked();
    void onStopButtonClicked();
    void onSkipForwardButtonClicked();
    void onBackButtonClicked();
    void onLeftButtonClicked();
    void onRightButtonClicked();

    void updateSleepDuration(int value);

    void onUpdatePositionOnSlider(int value);

private:
    enum class PlayingDirection
    {
        Forward = +1,
        Backward = -1
    };

    void playingRequested(PlayingDirection direction);

    void configureUIElements(int argc, char* argv[]);
    void setupConnections();
    void configureButtons();
    void configureButton(QPushButton* button, QStyle::StandardPixmap icon);
    void configureSliders();
    void configureCursorPosition();
    void initializeSceneWidget(int argc, char* argv[]);
    void setTotalStepsFromConfiguration(char *configurationFile);
    void connectButtons();
    void connectSliders();
    void connectMenuActions();
    void showInputFilePathOnBarLabel(const QString& inputFilePath);

    void loadStrings();

    void setPositionOnWidgets(int stepPosition, bool updateSlider=true);

    void setTotalSteps(int totalStepsValue);
    int totalSteps() const;

    void changeWhichButtonsAreEnabled();


    Ui::MainWindow* ui;

    int currentStep = 1;
    int cursorValueSleep = 1;
    int stepIncrement = 0;
    bool isPlaying = false ;
    bool isBacking = false;
    bool movingCursorSleep = false;

    QString noSelectionMessage;
    QString directorySelectionMessage;
    QString compilationSuccessfulMessage;
    QString compilationFailedMessage;
    QString deleteSuccessfulMessage;
    QString deleteFailedMessage;
};
