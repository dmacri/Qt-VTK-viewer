/** @file ClickableLabel.h
 * @brief Declaration of the ClickableLabel class widget (used in GUI), a QLabel with double click handling.
 * The class should contain path to config file. */

#pragma once

#include <QLabel>

class QMouseEvent;

/** @class ClickableLabel
 * @brief A QLabel that emits a signal when double-clicked.
 * 
 * This class extends QLabel to provide double-click functionality and file name association. */
class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    /// @brief Constructs a ClickableLabel with the given parent.
    explicit ClickableLabel(QWidget* parent = nullptr);

    /// @brief Sets the file name associated with this label. The filename is shown on the widged.
    void setFileName(QString fileName);

    /// @brief Returns the file name associated with this label.
    const QString& getFileName() const
    {
        return fileName;
    }

signals:
    /// @brief Emitted when the label is double-clicked.
    void doubleClicked();

protected:
    /// @brief Handles mouse double-click events.
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QString fileName;
};
