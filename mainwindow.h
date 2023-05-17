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

    void togglePlay(int numStep);


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();




private:
    Ui::MainWindow* ui;
    QTimer timer;
    QCommonStyle style;
    QString const styleSheet = "QPushButton {"
                         "  color: black;"          // Colore del testo
                         "  font-size: 16px;"     // Dimensione del testo
                         "  font-weight: bold;"   // Grassetto del testo
                         "}";


};

#endif // MAINWINDOW_H
