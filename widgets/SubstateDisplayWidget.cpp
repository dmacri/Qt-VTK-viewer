/** @file SubstateDisplayWidget.cpp
 * @brief Implementation of SubstateDisplayWidget. */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QEvent>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <limits>
#include <cmath>
#include <cctype>
#include "SubstateDisplayWidget.h"


SubstateDisplayWidget::SubstateDisplayWidget(const std::string& fieldName, QWidget* parent)
    : QWidget(parent)
    , m_fieldName(fieldName)
    , m_nameLabel(new QLabel(QString::fromStdString(fieldName)))
    , m_valueLabel(new QLabel("-"))
    , m_minSpinBox(new QDoubleSpinBox())
    , m_maxSpinBox(new QDoubleSpinBox())
    , m_formatLineEdit(new QLineEdit())
    , m_use3dButton(new QPushButton("Use as 3rd dimension"))
    , m_use2dButton(new QPushButton("Use as 2D"))
    , m_minColorLabel(new QLabel("Min:"))
    , m_minColorButton(new QPushButton())
    , m_maxColorLabel(new QLabel("Max:"))
    , m_maxColorButton(new QPushButton())
    , m_clearColorsButton(new QPushButton("Clear Colors"))
    , m_minColor("")
    , m_maxColor("")
{
    setupUI();
    connectSignals();
    
    // Enable context menu for the widget
    setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Install event filter on all child widgets to intercept right-click
    installEventFiltersOnChildren();
}

void SubstateDisplayWidget::setupUI()
{
    // Create main layout with border
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(4);

    // Set widget border
    setStyleSheet("SubstateDisplayWidget { border: 1px solid #cccccc; border-radius: 3px; background-color: #f9f9f9; }");

    // Title with field name (full width) - bold and larger
    auto titleLayout = new QHBoxLayout();
    auto titleLabel = new QLabel("Param:");
    titleLabel->setStyleSheet("QLabel { font-size: 8pt; }");
    titleLabel->setMaximumWidth(40);
    m_nameLabel->setStyleSheet("QLabel { font-size: 10pt; font-weight: bold; }");
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(m_nameLabel);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(titleLayout);

    // Value display (read-only) - full width
    auto valueLayout = new QHBoxLayout();
    auto valueTextLabel = new QLabel("Val:");
    valueTextLabel->setMaximumWidth(35);
    m_valueLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 1px; font-size: 9pt; }");
    m_valueLabel->setMaximumHeight(20);
    valueLayout->addWidget(valueTextLabel);
    valueLayout->addWidget(m_valueLabel);
    valueLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(valueLayout);

    // Two-column layout for Min and Max
    auto minMaxLayout = new QHBoxLayout();
    minMaxLayout->setSpacing(2);
    minMaxLayout->setContentsMargins(0, 0, 0, 0);

    // Minimum value (left column)
    auto minLayout = new QVBoxLayout();
    auto minTextLabel = new QLabel("Min:");
    minTextLabel->setStyleSheet("QLabel { font-size: 8pt; }");
    m_minSpinBox->setRange(-1e9, 1e9);
    m_minSpinBox->setDecimals(2);
    m_minSpinBox->setSpecialValueText(" ");  // Empty display for minimum value
    m_minSpinBox->setValue(m_minSpinBox->minimum());  // Start with "empty" state
    m_minSpinBox->setMaximumHeight(20);
    m_minSpinBox->setStyleSheet("QDoubleSpinBox { font-size: 8pt; }");
    minLayout->addWidget(minTextLabel);
    minLayout->addWidget(m_minSpinBox);
    minLayout->setContentsMargins(0, 0, 0, 0);
    minLayout->setSpacing(0);
    minMaxLayout->addLayout(minLayout);

    // Maximum value (right column)
    auto maxLayout = new QVBoxLayout();
    auto maxTextLabel = new QLabel("Max:");
    maxTextLabel->setStyleSheet("QLabel { font-size: 8pt; }");
    m_maxSpinBox->setRange(-1e9, 1e9);
    m_maxSpinBox->setDecimals(2);
    m_maxSpinBox->setSpecialValueText(" ");  // Empty display for minimum value
    m_maxSpinBox->setValue(m_maxSpinBox->minimum());  // Start with "empty" state
    m_maxSpinBox->setMaximumHeight(20);
    m_maxSpinBox->setStyleSheet("QDoubleSpinBox { font-size: 8pt; }");
    maxLayout->addWidget(maxTextLabel);
    maxLayout->addWidget(m_maxSpinBox);
    maxLayout->setContentsMargins(0, 0, 0, 0);
    maxLayout->setSpacing(0);
    minMaxLayout->addLayout(maxLayout);

    mainLayout->addLayout(minMaxLayout);

    // Format string (full width)
    auto formatLayout = new QHBoxLayout();
    auto formatTextLabel = new QLabel("Fmt:");
    formatTextLabel->setMaximumWidth(35);
    m_formatLineEdit->setMaximumHeight(20);
    m_formatLineEdit->setStyleSheet("QLineEdit { font-size: 8pt; }");
    formatLayout->addWidget(formatTextLabel);
    formatLayout->addWidget(m_formatLineEdit);
    formatLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(formatLayout);

    // Buttons layout (3D, 2D)
    auto buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(2);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    
    // Use as 3rd dimension button
    m_use3dButton->setMaximumHeight(22);
    m_use3dButton->setStyleSheet("QPushButton { font-size: 8pt; padding: 2px; }");
    buttonsLayout->addWidget(m_use3dButton);
    
    // Use as 2D button
    m_use2dButton->setMaximumHeight(22);
    m_use2dButton->setStyleSheet("QPushButton { font-size: 8pt; padding: 2px; background-color: #e8f4f8; }");
    buttonsLayout->addWidget(m_use2dButton);
    
    mainLayout->addLayout(buttonsLayout);

    // Colors layout
    auto colorsLayout = new QHBoxLayout();
    colorsLayout->setSpacing(4);
    colorsLayout->setContentsMargins(0, 0, 0, 0);
    
    // Min color
    m_minColorLabel->setMaximumWidth(30);
    colorsLayout->addWidget(m_minColorLabel);
    m_minColorButton->setMaximumHeight(20);
    m_minColorButton->setMaximumWidth(40);
    m_minColorButton->setToolTip("Click to set minimum value color");
    colorsLayout->addWidget(m_minColorButton);
    
    // Max color
    m_maxColorLabel->setMaximumWidth(30);
    colorsLayout->addWidget(m_maxColorLabel);
    m_maxColorButton->setMaximumHeight(20);
    m_maxColorButton->setMaximumWidth(40);
    m_maxColorButton->setToolTip("Click to set maximum value color");
    colorsLayout->addWidget(m_maxColorButton);
    
    // Clear colors button
    m_clearColorsButton->setMaximumHeight(20);
    m_clearColorsButton->setStyleSheet("QPushButton { font-size: 7pt; padding: 1px; }");
    colorsLayout->addWidget(m_clearColorsButton);
    
    colorsLayout->addStretch();
    mainLayout->addLayout(colorsLayout);

    setLayout(mainLayout);
}

void SubstateDisplayWidget::connectSignals()
{
    connect(m_use3dButton, &QPushButton::clicked, this, [this]() {
        emit use3rdDimensionRequested(m_fieldName);
    });

    connect(m_use2dButton, &QPushButton::clicked, this, &SubstateDisplayWidget::onUse2DClicked);

    // Connect color buttons
    connect(m_minColorButton, &QPushButton::clicked, this, &SubstateDisplayWidget::onMinColorClicked);
    connect(m_maxColorButton, &QPushButton::clicked, this, &SubstateDisplayWidget::onMaxColorClicked);
    connect(m_clearColorsButton, &QPushButton::clicked, this, &SubstateDisplayWidget::onClearColorsClicked);

    // Connect spinbox value changes to update button state
    connect(m_minSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SubstateDisplayWidget::updateButtonState);
    connect(m_maxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SubstateDisplayWidget::updateButtonState);

    // Install event filters on spinboxes to detect focus out
    m_minSpinBox->installEventFilter(this);
    m_maxSpinBox->installEventFilter(this);

    // Initial button state
    updateButtonState();
    updateColorButtonAppearance();
}

void SubstateDisplayWidget::setCellValue(const std::string& value)
{
    // If format is set, try to parse and reformat the value
    std::string format = getFormat();
    if (!format.empty())
    {
        try
        {
            // Try to parse value as double
            double numValue = std::stod(value);
            
            // Determine format type
            bool isDecimal = format.find('f') != std::string::npos || 
                           format.find('e') != std::string::npos ||
                           format.find('g') != std::string::npos;
            bool isInteger = format.find('d') != std::string::npos ||
                           format.find('i') != std::string::npos;
            
            if (isDecimal)
            {
                // Format as decimal - extract precision if specified
                int precision = 2;  // Default precision
                size_t dotPos = format.find('.');
                if (dotPos != std::string::npos && dotPos + 1 < format.length())
                {
                    char precChar = format[dotPos + 1];
                    if (std::isdigit(precChar))
                        precision = precChar - '0';
                }
                m_valueLabel->setText(QString::number(numValue, 'f', precision));
            }
            else if (isInteger)
            {
                // Format as integer
                m_valueLabel->setText(QString::number(static_cast<long long>(numValue)));
            }
            else
            {
                // Default: show as-is
                m_valueLabel->setText(QString::fromStdString(value));
            }
        }
        catch (const std::exception&)
        {
            // If parsing fails, show value as-is
            m_valueLabel->setText(QString::fromStdString(value));
        }
    }
    else
    {
        // No format specified, show value as-is
        m_valueLabel->setText(QString::fromStdString(value));
    }
}

double SubstateDisplayWidget::getMinValue() const
{
    if (m_minSpinBox->value() == m_minSpinBox->minimum())
        return std::numeric_limits<double>::quiet_NaN();
    return m_minSpinBox->value();
}

void SubstateDisplayWidget::setMinValue(double value)
{
    if (std::isnan(value))
        m_minSpinBox->setValue(m_minSpinBox->minimum());  // Empty state
    else
        m_minSpinBox->setValue(value);
}

double SubstateDisplayWidget::getMaxValue() const
{
    if (m_maxSpinBox->value() == m_maxSpinBox->minimum())
        return std::numeric_limits<double>::quiet_NaN();
    return m_maxSpinBox->value();
}

void SubstateDisplayWidget::setMaxValue(double value)
{
    if (std::isnan(value))
        m_maxSpinBox->setValue(m_maxSpinBox->minimum());  // Empty state
    else
        m_maxSpinBox->setValue(value);
}

std::string SubstateDisplayWidget::getFormat() const
{
    auto text = m_formatLineEdit->text().toStdString();
    // Remove % prefix if present
    if (!text.empty() && text[0] == '%')
        return text.substr(1);
    return text;
}

void SubstateDisplayWidget::setFormat(const std::string& format)
{
    // Remove % prefix if present in input
    std::string cleanFormat = format;
    if (!cleanFormat.empty() && cleanFormat[0] == '%')
        cleanFormat = cleanFormat.substr(1);
    m_formatLineEdit->setText(QString::fromStdString(cleanFormat));
}

bool SubstateDisplayWidget::hasMinValue() const
{
    return m_minSpinBox->value() != m_minSpinBox->minimum();
}

bool SubstateDisplayWidget::hasMaxValue() const
{
    return m_maxSpinBox->value() != m_maxSpinBox->minimum();
}

void SubstateDisplayWidget::updateButtonState()
{
    // Button is enabled only if both min and max values are set
    const bool hasMin = hasMinValue();
    const bool hasMax = hasMaxValue();
    const bool isEnabled = hasMin && hasMax;
    
    m_use3dButton->setEnabled(isEnabled);
    
    // Update tooltip to explain why button might be disabled
    if (isEnabled)
    {
        m_use3dButton->setToolTip("Use this field as 3rd dimension in 3D visualization");
    }
    else
    {
        m_use3dButton->setToolTip("Set both Min and Max values to enable 3D visualization");
    }

    // Emit signal with current min/max values so they can be stored in substateInfo
    emit minMaxValuesChanged(m_fieldName, getMinValue(), getMaxValue());
}

void SubstateDisplayWidget::installEventFiltersOnChildren()
{
    // Install event filter on all child widgets to intercept right-click
    m_minSpinBox->installEventFilter(this);
    m_maxSpinBox->installEventFilter(this);
    m_formatLineEdit->installEventFilter(this);
    m_use3dButton->installEventFilter(this);
    m_nameLabel->installEventFilter(this);
    m_valueLabel->installEventFilter(this);
}

bool SubstateDisplayWidget::eventFilter(QObject* obj, QEvent* event)
{
    // Intercept right-click (context menu) on child widgets
    if (event->type() == QEvent::ContextMenu)
    {
        // Forward context menu event to this widget's contextMenuEvent
        auto contextEvent = static_cast<QContextMenuEvent*>(event);
        contextMenuEvent(contextEvent);
        return true;  // Event handled
    }
    
    // Intercept focus out on spinboxes to trigger visualization refresh
    if (event->type() == QEvent::FocusOut)
    {
        if (obj == m_minSpinBox)
        {
            onMinSpinBoxFocusOut();
            return false;  // Let the event pass through
        }
        else if (obj == m_maxSpinBox)
        {
            onMaxSpinBoxFocusOut();
            return false;  // Let the event pass through
        }
    }
    
    // Let other events pass through
    return QWidget::eventFilter(obj, event);
}

void SubstateDisplayWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // Create context menu for the widget
    QMenu menu;
    
    // Add calculation actions with icons
    auto calcMinAction = menu.addAction(QIcon(":/icons/zoom_to.png"), "Calculate minimum");
    connect(calcMinAction, &QAction::triggered, this, &SubstateDisplayWidget::onCalculateMinimum);
    
    auto calcMinGtZeroAction = menu.addAction(QIcon(":/icons/zoom_to.png"), "Calculate minimum > 0");
    connect(calcMinGtZeroAction, &QAction::triggered, this, &SubstateDisplayWidget::onCalculateMinimumGreaterThanZero);
    
    menu.addSeparator();
    
    auto calcMaxAction = menu.addAction(QIcon(":/icons/zoom_to.png"), "Calculate maximum");
    connect(calcMaxAction, &QAction::triggered, this, &SubstateDisplayWidget::onCalculateMaximum);

    menu.addSeparator();

    auto calcMaxAndMinGtZeroAction = menu.addAction(QIcon(":/icons/zoom_to.png"), "Calculate maximum and minimum > 0");
    connect(calcMaxAndMinGtZeroAction, &QAction::triggered, this, &SubstateDisplayWidget::onCalculateMinimumGreaterThanZeroAndMaximum);
    
    // Show menu at cursor position
    menu.exec(event->globalPos());
}

void SubstateDisplayWidget::onCalculateMinimum()
{
    emit calculateMinimumRequested(m_fieldName);
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onCalculateMinimumGreaterThanZero()
{
    emit calculateMinimumGreaterThanZeroRequested(m_fieldName);
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onCalculateMaximum()
{
    emit calculateMaximumRequested(m_fieldName);
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onCalculateMinimumGreaterThanZeroAndMaximum()
{
    emit onCalculateMinimumGreaterThanZero();
    emit onCalculateMaximum();
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onUse2DClicked()
{
    emit use2DRequested(m_fieldName);
}

void SubstateDisplayWidget::setMinColor(const std::string& color)
{
    m_minColor = color;
    updateColorButtonAppearance();
    emit colorsChanged(m_fieldName, m_minColor, m_maxColor);
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::setMaxColor(const std::string& color)
{
    m_maxColor = color;
    updateColorButtonAppearance();
    emit colorsChanged(m_fieldName, m_minColor, m_maxColor);
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onMinColorClicked()
{
    QColor currentColor = m_minColor.empty() ? QColor(Qt::white) : QColor(QString::fromStdString(m_minColor));
    QColor selectedColor = QColorDialog::getColor(currentColor, this, "Select minimum value color");
    
    if (selectedColor.isValid())
    {
        setMinColor(selectedColor.name().toStdString());
    }
}

void SubstateDisplayWidget::onMaxColorClicked()
{
    QColor currentColor = m_maxColor.empty() ? QColor(Qt::white) : QColor(QString::fromStdString(m_maxColor));
    QColor selectedColor = QColorDialog::getColor(currentColor, this, "Select maximum value color");
    
    if (selectedColor.isValid())
    {
        setMaxColor(selectedColor.name().toStdString());
    }
}

void SubstateDisplayWidget::onClearColorsClicked()
{
    setMinColor("");
    setMaxColor("");
}

void SubstateDisplayWidget::updateColorButtonAppearance()
{
    // Min color button
    if (m_minColor.empty())
    {
        m_minColorButton->setStyleSheet("QPushButton { background-color: #cccccc; border: 1px solid #999999; }");
        m_minColorButton->setToolTip("Click to set minimum value color (currently inactive)");
    }
    else
    {
        m_minColorButton->setStyleSheet(QString("QPushButton { background-color: %1; border: 1px solid #000000; }").arg(QString::fromStdString(m_minColor)));
        m_minColorButton->setToolTip(QString("Min color: %1").arg(QString::fromStdString(m_minColor)));
    }
    
    // Max color button
    if (m_maxColor.empty())
    {
        m_maxColorButton->setStyleSheet("QPushButton { background-color: #cccccc; border: 1px solid #999999; }");
        m_maxColorButton->setToolTip("Click to set maximum value color (currently inactive)");
    }
    else
    {
        m_maxColorButton->setStyleSheet(QString("QPushButton { background-color: %1; border: 1px solid #000000; }").arg(QString::fromStdString(m_maxColor)));
        m_maxColorButton->setToolTip(QString("Max color: %1").arg(QString::fromStdString(m_maxColor)));
    }
}

void SubstateDisplayWidget::onMinSpinBoxFocusOut()
{
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::onMaxSpinBoxFocusOut()
{
    emit visualizationRefreshRequested();
}

void SubstateDisplayWidget::setActive(bool active)
{
    if (active)
    {
        // Highlight with light blue background
        setStyleSheet("background-color: #E3F2FD; border: 2px solid #2196F3; border-radius: 4px; padding: 2px;");
    }
    else
    {
        // Remove highlight
        setStyleSheet("");
    }
}
