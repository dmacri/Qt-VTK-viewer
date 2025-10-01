#include <QTableWidget>
#include <QLabel>
#include <QFont>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include "ConfigDetailsDialog.h"
#include "config/Config.h"


ConfigDetailsDialog::ConfigDetailsDialog(const std::string& configFilePath, QWidget* parent)
    : QDialog(parent)
    , tableWidget(new QTableWidget(this))
    , mainLayout(new QVBoxLayout(this))
{
    setupUI();
    loadConfigData(configFilePath);
}

ConfigDetailsDialog::~ConfigDetailsDialog() = default;

void ConfigDetailsDialog::setupUI()
{
    setWindowTitle(tr("Configuration Details"));
    setMinimumSize(600, 600);
    
    // Setup table
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    
    // Add table to layout
    mainLayout->addWidget(tableWidget);
    
    // Add close button
    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeButton);
    
    setLayout(mainLayout);
}

void ConfigDetailsDialog::loadConfigData(const std::string& configFilePath)
{
    try
    {
        Config config(configFilePath);
        config.readConfigFile();
        
        int rowCount = 0;
        
        // Count total rows needed
        const auto categoryNames = config.categoryNames();
        
        for (const auto& categoryName : categoryNames)
        {
            ConfigCategory* category = config.getConfigCategory(categoryName);
            if (category && category->getSize() > 0)
            {
                rowCount += 1 + category->getSize(); // 1 for category header + parameters
            }
        }
        
        tableWidget->setRowCount(rowCount);
        
        int currentRow = 0;
        
        // Populate table
        for (const auto& categoryName : categoryNames)
        {
            ConfigCategory* category = config.getConfigCategory(categoryName);
            if (!category || category->getSize() == 0)
                continue;
            
            // Add category header row
            QTableWidgetItem* categoryItem = new QTableWidgetItem(QString::fromStdString(categoryName + ":"));
            QFont boldFont;
            boldFont.setBold(true);
            categoryItem->setFont(boldFont);
            categoryItem->setBackground(QColor(220, 220, 220));
            
            tableWidget->setItem(currentRow, 0, categoryItem);
            tableWidget->setItem(currentRow, 1, new QTableWidgetItem(""));
            tableWidget->item(currentRow, 1)->setBackground(QColor(220, 220, 220));
            tableWidget->setSpan(currentRow, 0, 1, 2);
            currentRow++;
            
            // Add parameters
            for (const auto& param : category->getConfigParameters())
            {
                QTableWidgetItem* nameItem = new QTableWidgetItem(
                    QString("    ") + QString::fromStdString(param.getName())
                );
                QTableWidgetItem* valueItem = new QTableWidgetItem(
                    QString::fromStdString(param.getDefaultValue())
                );
                
                tableWidget->setItem(currentRow, 0, nameItem);
                tableWidget->setItem(currentRow, 1, valueItem);
                currentRow++;
            }
        }
    }
    catch (const std::exception& e)
    {
        // If config reading fails, show error in table
        tableWidget->setRowCount(1);
        tableWidget->setItem(0, 0, new QTableWidgetItem(tr("Error loading configuration")));
        tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(e.what())));
    }
}
