#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QIntValidator>

#include <vtkDataSetReader.h>


MainWindow::MainWindow(QWidget* parent, int argc, char *argv[]) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sceneWidget->addVisualizer(argc,argv);



}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showAboutDialog()
{

    QMessageBox::information(
                this, "About",
                "By Davide Macri.\n Configurator for  visualizer");
}

void MainWindow::showOpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                                    "VTK Files (*.vtk)");

    // Open file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Return on Cancel
    if (!file.exists())
        return;


}


void MainWindow::showVisualizerWindows(int argc, char* argv[]){
    ui->sceneWidget->addVisualizer(argc,argv);
}


void MainWindow::on_pushButton_clicked()
{
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->sceneWidget->decreaseCountDown();
}


void MainWindow::on_pushButton_3_clicked()
{
    QIntValidator *validator = new QIntValidator(ui->lineEdit);
    ui->lineEdit->setValidator(validator);
    string textEdited = (string) ui->lineEdit->text().toUtf8().constData();
    ui->sceneWidget->selectedStepParameter(textEdited);
}

void MainWindow::on_lineEdit_returnPressed()
{

}

