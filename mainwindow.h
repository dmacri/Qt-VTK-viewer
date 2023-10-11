#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcommonstyle.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0,int argc=0, char* argv[]=nullptr);
    ~MainWindow();
    QStringList readNLinesFromFile(const QString& filePath);

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    void togglePlay();





private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void handleButtonClick();

    void updateSleepDuration(int value);

private:
    Ui::MainWindow* ui;
    QTimer timer;
    int totalSteps = 0;
    int currentStep = 1;
    int sleepDuration = 100;
    QCommonStyle style;
    bool isIterating = false;
    bool isPlaying=false ;
    bool isBacking = false;

    void configureUIElements(int argc, char* argv[]);
    void setupConnections();
    void configureLineEdit();
    void configureButtons();
    void configureButton(QPushButton* button, QStyle::StandardPixmap icon, const QString& styleSheet);
    void configureSliders();
    void initializeSceneWidget(int argc, char* argv[]);
    void setTotalStepsFromConfiguration(const char* configurationFile);
    void connectButtons();
    void connectButton(QPushButton* button);
    void connectSliders();

    void loadStrings();
    QString noSelectionMessage;
    QString directorySelectionMessage;
    QString compilationSuccessfulMessage;
    QString compilationFailedMessage;
    QString deleteSuccessfulMessage;
    QString deleteFailedMessage;

    QString const styleButtonGotoStep="QPushButton {"
                                      "    color: black;"
                                      "    font-size: 16px;"
                                      "    font-weight: bold;"
                                      "    margin: 5px;"  // Aggiungi uno spazio di 5px tra i bottoni
                                      "    background-color: #c0c0c0;"
                                      "    border: none;"
                                      "    border-radius: 20px;"
                                      "    padding: 10px 20px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "    background-color: #a0a0a0;"
                                      "}"
                                      "QPushButton:pressed {"
                                      "    background-color: #888888;"
                                      "}";
    QString const styleSheet = "QPushButton {"
                                       "    color: black;"
                                       "    font-size: 16px;"
                                       "    font-weight: bold;"
                                       "    margin: 5px;"  // Aggiungi uno spazio di 5px tra i bottoni
            "}"
            "QPushButton:hover {"
            "    background-color: #c0c0c0;"
            "    border: 1px solid #a0a0a0;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #a0a0a0;"
            "    border: 1px solid #a0a0a0;"
            "}";
    QString const styleSheetButtonLeftColumn="QPushButton {"
                                             "    color: black;"
                                             "    font-size: 16px;"
                                             "    font-weight: bold;"
                                             "    padding-left: 30px;"
                                             "    padding-right: 10px;"
                                             "    text-align: left;"
                                             "    background-color: transparent;"
                                             "    border: 2px solid #808080;"  // Bordo con colore grigio (#808080)
            "    border-radius: 25px;"  // Angoli arrotondati
            "}"
            "QPushButton:hover {"
            "    background-color: #c0c0c0;"  // Sfondo grigio chiaro (#c0c0c0)
            "    border-color: #a0a0a0;"  // Colore del bordo grigio scuro (#a0a0a0)
            "}"
            "QPushButton:pressed {"
            "    background-color: #a0a0a0;"  // Sfondo grigio scuro (#a0a0a0)
            "    border-color: #808080;"  // Colore del bordo grigio (#808080)
            "}";

    QString const styleSheetSleep = "QSlider {"
                                    "    min-height: 20px;"
                                    "}"
                                    "QSlider::groove {"
                                    "    border: 1px solid #bbb;"
                                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                    "                                  stop:0 green, stop:0.3 green,"
                                    "                                  stop:0.3 green, stop:0.7 green,"
                                    "                                  stop:0.7 red, stop:1 red);"
                                    "    margin: 0px 10px;"
                                    "}"
                                    "QSlider::handle {"
                                    "    background: white;"
                                    "    border: 1px solid #777;"
                                    "    width: 20px;"
                                    "    margin: -5px -10px;"
                                    "    border-radius: 10px;"
                                    "}"
                                    "QSlider::handle:hover {"
                                    "    background: #ccc;"
                                    "    border-color: #888;"
                                    "}"
                                    "QSlider::sub-page {"
                                    "    background: transparent;"
                                    "    border: none;"
                                    "}"
                                    "QSlider::add-page {"
                                    "    background: transparent;"
                                    "    border: none;"
                                    "}";


};

#endif // MAINWINDOW_H
