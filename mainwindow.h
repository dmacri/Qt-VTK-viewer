#pragma once

#include <QCommonStyle>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QPushButton;


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(int argc, char* argv[], QWidget* parent = nullptr);
    ~MainWindow();

    static QStringList readNLinesFromFile(const QString& filePath); // TODO: GB: not used?

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    void togglePlay();

private slots:
    void onStepNumberInputed();

    void onPlayButtonClicked();
    void onStopButtonClicked();
    void onSkipForwardButtonClicked();
    void onBackButtonClicked();
    void onLeftButtonClicked();
    void onRightButtonClicked();

    void updateSleepDuration(int value);

    void updatePosition(int value);

private:
    void configureUIElements(int argc, char* argv[]);
    void setupConnections();
    void addValidatorForPositionInputWidget();
    void configureButtons();
    void configureButton(QPushButton* button, QStyle::StandardPixmap icon);
    void configureSliders();
    void configureCursorPosition();
    void initializeSceneWidget(int argc, char* argv[]);
    void setTotalStepsFromConfiguration(char *configurationFile);
    void connectButtons();
    void connectSliders();
    void showInputFilePathOnBarLabel(const QString& inputFilePath);

    void loadStrings();

    void setPositionOnWidgets(int stepPosition);


    Ui::MainWindow* ui;

    int totalSteps = 0;
    int currentStep = 1;
    int cursorValueSleep = 1;
    int stepIncrement = 0;
    bool isPlaying = false ;
    bool isBacking = false;
    bool movingCursorSleep = false;
    bool updateValueAndPositionWithStep = true;

    QString noSelectionMessage;
    QString directorySelectionMessage;
    QString compilationSuccessfulMessage;
    QString compilationFailedMessage;
    QString deleteSuccessfulMessage;
    QString deleteFailedMessage;

    // QString const styleButtonGotoStep="QPushButton {"
    //                                     "    color: black;"
    //                                     "    font-size: 16px;"
    //                                     "    font-weight: bold;"
    //                                     "    margin: 5px;"  // Aggiungi uno spazio di 5px tra i bottoni
    //                                     "    background-color: #c0c0c0;"
    //                                     "    border: none;"
    //                                     "    border-radius: 20px;"
    //                                     "    padding: 10px 20px;"
    //                                     "}"
    //                                     "QPushButton:hover {"
    //                                     "    background-color: #a0a0a0;"
    //                                     "}"
    //                                     "QPushButton:pressed {"
    //                                     "    background-color: #888888;"
    //                                     "}";  // TODO: GB: not used?
    // QString const styleSheet = "QPushButton {"
    //                            "    color: black;"
    //                            "    font-size: 16px;"
    //                            "    font-weight: bold;"
    //                            "    margin: 5px;"
    //                            "}"
    //                            "QPushButton:hover {"
    //                            "    background-color: #c0c0c0;"
    //                            "    border: 1px solid #a0a0a0;"
    //                            "}"
    //                            "QPushButton:pressed {"
    //                            "    background-color: #a0a0a0;"
    //                            "    border: 1px solid #a0a0a0;"
    //                            "}";  // TODO: GB: not used?
    // QString const styleSheetButtonLeftColumn="QPushButton {"
    //                                            "    color: black;"
    //                                            "    font-size: 16px;"
    //                                            "    font-weight: bold;"
    //                                            "    padding-left: 30px;"
    //                                            "    padding-right: 10px;"
    //                                            "    text-align: left;"
    //                                            "    background-color: transparent;"
    //                                            "    border: 2px solid #808080;"  // Bordo con colore grigio (#808080)
    //                                            "    border-radius: 25px;"  // Angoli arrotondati
    //                                            "}"
    //                                            "QPushButton:hover {"
    //                                            "    background-color: #c0c0c0;"  // Sfondo grigio chiaro (#c0c0c0)
    //                                            "    border-color: #a0a0a0;"  // Colore del bordo grigio scuro (#a0a0a0)
    //                                            "}"
    //                                            "QPushButton:pressed {"
    //                                            "    background-color: #a0a0a0;"  // Sfondo grigio scuro (#a0a0a0)
    //                                            "    border-color: #808080;"  // Colore del bordo grigio (#808080)
    //                                            "}";  // TODO: GB: not used?

    // QString const styleSheetSleep = "QSlider {"
    //                                 "    min-height: 20px;"
    //                                 "}"
    //                                 "QSlider::groove {"
    //                                 "    border: 1px solid #bbb;"
    //                                 "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
    //                                 "                                  stop:0 #aaffaa, stop:1 #006600);"
    //                                 "    margin: 0px 10px;"
    //                                 "}"
    //                                 "QSlider::handle {"
    //                                 "    background: white;"
    //                                 "    border: 1px solid #777;"
    //                                 "    width: 20px;"
    //                                 "    margin: -5px -10px;"
    //                                 "    border-radius: 10px;"
    //                                 "}"
    //                                 "QSlider::handle:hover {"
    //                                 "    background: #ccc;"
    //                                 "    border-color: #888;"
    //                                 "}"
    //                                 "QSlider::sub-page {"
    //                                 "    background: transparent;"
    //                                 "    border: none;"
    //                                 "}"
    //                                 "QSlider::add-page {"
    //                                 "    background: transparent;"
    //                                 "    border: none;"
    //                                 "}";  // TODO: GB: not used?
};
