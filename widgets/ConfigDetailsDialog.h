/** @file ConfigDetailsDialog.h
 * @brief Declaration of the ConfigDetailsDialog class for displaying configuration details. */

#pragma once

#include <string>
#include <QDialog>

class QTableWidget;
class QVBoxLayout;
class QLabel;

/** @class ConfigDetailsDialog
 * @brief A dialog that displays detailed configuration parameters in a table format.
 * 
 * This dialog shows configuration parameters loaded from a file in a user-friendly table view. */
class ConfigDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    /** @brief Constructs a ConfigDetailsDialog with the specified configuration file. 
     *  @param configFilePath Path to the configuration file to display
     * @param parent The parent widget */
    explicit ConfigDetailsDialog(const std::string& configFilePath, QWidget* parent = nullptr);
    
    /// @brief Destroys the ConfigDetailsDialog.
    ~ConfigDetailsDialog();

private:
    /// @brief Sets up the user interface components.
    void setupUI();
    
    /// @brief Loads configuration data from the specified file.
    /// @param configFilePath Path to the configuration file
    void loadConfigData(const std::string& configFilePath);
    
    /// @brief Adjusts the dialog size to fit its content (to make sure that all rows are visible)
    void adjustSizeToContent();
    
    QTableWidget* tableWidget;  ///< Table widget for displaying configuration parameters
    QVBoxLayout* mainLayout;    ///< Main layout of the dialog
    QLabel* filePathLabel;      ///< Label displaying the configuration file path
};
