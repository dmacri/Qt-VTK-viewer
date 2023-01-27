#include "mainwindow.h"
#include "QtVtkViewer_autogen/include/ui_mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <vtkDataSetReader.h>

#include "arrowpad.h"


MainWindow::MainWindow(QWidget* parent, int argc, char *argv[]) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sceneWidget->addVisualizer(argc,argv);
    arrowPad = new ArrowPad;
   // setCentralWidget(arrowPad);

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showAboutDialog()
{

    QMessageBox::information(
                this, "About",
                "By Martijn Koopman.\nSource code available under Apache License 2.0.");
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

    openFile(fileName);
}


void MainWindow::showVisualizerWindows(int argc, char* argv[]){
    ui->sceneWidget->addVisualizer(argc,argv);
}

void MainWindow::openFile(const QString& fileName)
{
    ui->sceneWidget->removeDataSet();

    // Create reader
    vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
    reader->SetFileName(fileName.toStdString().c_str());

    // Read the file
    reader->Update();

    // Add data set to 3D view
    vtkSmartPointer<vtkDataSet> dataSet = reader->GetOutput();
    if (dataSet != nullptr) {
        ui->sceneWidget->addDataSet(reader->GetOutput());
    }
}


void MainWindow::on_pushButton_clicked()
{
   // ui->sceneWidget->increaseCountUp();
}


void MainWindow::on_pushButton_2_clicked()
{
   ui->sceneWidget->decreaseCountDown();
}


void MainWindow::on_pushButton_3_clicked()
{
  string textEdited = (string) ui->lineEdit->text().toUtf8().constData();
  ui->sceneWidget->selectedStepParameter(textEdited);
}

