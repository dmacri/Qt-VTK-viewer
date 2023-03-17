#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0,int argc=0, char* argv[]=nullptr);
    ~MainWindow();

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    //! Show the 'Open visualizer' dialog
    void showVisualizerWindows(int argc, char *argv[]);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


private:
    Ui::MainWindow* ui;

};

#endif // MAINWINDOW_H
