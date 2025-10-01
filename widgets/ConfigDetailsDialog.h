#pragma once

#include <string>
#include <QDialog>

class QTableWidget;
class QVBoxLayout;


class ConfigDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDetailsDialog(const std::string& configFilePath, QWidget* parent = nullptr);
    ~ConfigDetailsDialog();

private:
    void setupUI();
    void loadConfigData(const std::string& configFilePath);
    
    QTableWidget* tableWidget;
    QVBoxLayout* mainLayout;
};
