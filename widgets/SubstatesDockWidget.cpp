/** @file SubstatesDockWidget.cpp
 * @brief Implementation of SubstatesDockWidget. */

#include <QVBoxLayout>
#include "SubstatesDockWidget.h"
#include "SubstateDisplayWidget.h"
#include "visualiser/SettingParameter.h"
#include "visualiserProxy/ISceneWidgetVisualizer.h"


SubstatesDockWidget::SubstatesDockWidget(QWidget* parent)
    : QDockWidget("Substate Parameters", parent)
    , m_scrollArea(new QScrollArea())
    , m_containerWidget(new QWidget())
    , m_containerLayout(new QVBoxLayout(m_containerWidget))
{
    // Setup scroll area
    m_scrollArea->setWidget(m_containerWidget);
    m_scrollArea->setWidgetResizable(true);

    // Setup container layout
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_containerLayout->setSpacing(10);

    // Set the scroll area as the main widget
    setWidget(m_scrollArea);

    // Set initial size
    setMinimumWidth(300);
    setMaximumWidth(400);
}

void SubstatesDockWidget::updateSubstates(SettingParameter* settingParameter)
{
    if (!settingParameter)
        return;

    // Clear existing widgets
    clearWidgets();

    // Get substate fields
    auto fields = settingParameter->getSubstateFields();

    // Create new widgets for each field
    for (const auto& field : fields)
    {
        auto widget = new SubstateDisplayWidget(field, m_containerWidget);

        // Restore saved parameters if they exist
        auto it = settingParameter->substateInfo.find(field);
        if (it != settingParameter->substateInfo.end())
        {
            widget->setMinValue(it->second.minValue);
            widget->setMaxValue(it->second.maxValue);
            widget->setFormat(it->second.format);
        }

        // Connect signal
        connect(widget, &SubstateDisplayWidget::use3rdDimensionRequested,
                this, &SubstatesDockWidget::use3rdDimensionRequested);

        m_containerLayout->addWidget(widget);
        m_substateWidgets[field] = widget;
    }

    // Add stretch at the end to push widgets to the top
    m_containerLayout->addStretch();
}

void SubstatesDockWidget::updateCellValues(SettingParameter* settingParameter, int row, int col, class ISceneWidgetVisualizer* visualizer)
{
    if (!settingParameter || !visualizer)
        return;

    // Update each substate widget with the cell value
    for (auto& pair : m_substateWidgets)
    {
        const auto& fieldName = pair.first;
        auto& widget = pair.second;

        // Get the cell value for this field
        std::string cellValue = visualizer->getCellStringEncoding(row, col, fieldName.c_str());
        if (!cellValue.empty())
        {
            widget->setCellValue(cellValue);
        }
    }
}

void SubstatesDockWidget::saveParametersToSettings(SettingParameter* settingParameter)
{
    if (!settingParameter)
        return;

    // Save all widget values back to substateInfo
    for (const auto& pair : m_substateWidgets)
    {
        const auto& fieldName = pair.first;
        const auto& widget = pair.second;

        auto it = settingParameter->substateInfo.find(fieldName);
        if (it != settingParameter->substateInfo.end())
        {
            it->second.minValue = widget->getMinValue();
            it->second.maxValue = widget->getMaxValue();
            it->second.format = widget->getFormat();
        }
    }
}

void SubstatesDockWidget::clearWidgets()
{
    // Remove all widgets from layout
    while (QLayoutItem* item = m_containerLayout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
    m_substateWidgets.clear();
}
