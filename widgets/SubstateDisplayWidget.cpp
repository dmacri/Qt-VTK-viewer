/** @file SubstateDisplayWidget.cpp
 * @brief Implementation of SubstateDisplayWidget. */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "SubstateDisplayWidget.h"


SubstateDisplayWidget::SubstateDisplayWidget(const std::string& fieldName, QWidget* parent)
    : QWidget(parent)
    , m_fieldName(fieldName)
    , m_nameLabel(new QLabel(QString::fromStdString(fieldName)))
    , m_valueLabel(new QLabel("0.00"))
    , m_minSpinBox(new QDoubleSpinBox())
    , m_maxSpinBox(new QDoubleSpinBox())
    , m_formatLineEdit(new QLineEdit("%f"))
    , m_use3dButton(new QPushButton("Use as 3rd dimension"))
{
    setupUI();
    connectSignals();
}

void SubstateDisplayWidget::setupUI()
{
    // Create main layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->setSpacing(3);

    // Title with field name (full width)
    auto titleLayout = new QHBoxLayout();
    auto titleLabel = new QLabel("Param:");
    titleLabel->setMaximumWidth(35);
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
    m_minSpinBox->setValue(0.0);
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
    m_maxSpinBox->setValue(0.0);
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

    // Use as 3rd dimension button
    m_use3dButton->setMaximumHeight(22);
    m_use3dButton->setStyleSheet("QPushButton { font-size: 8pt; padding: 2px; }");
    mainLayout->addWidget(m_use3dButton);

    setLayout(mainLayout);
}

void SubstateDisplayWidget::connectSignals()
{
    connect(m_use3dButton, &QPushButton::clicked, this, [this]() {
        emit use3rdDimensionRequested(m_fieldName);
    });
}

void SubstateDisplayWidget::setCellValue(const std::string& value)
{
    m_valueLabel->setText(QString::fromStdString(value));
}

double SubstateDisplayWidget::getMinValue() const
{
    return m_minSpinBox->value();
}

void SubstateDisplayWidget::setMinValue(double value)
{
    m_minSpinBox->setValue(value);
}

double SubstateDisplayWidget::getMaxValue() const
{
    return m_maxSpinBox->value();
}

void SubstateDisplayWidget::setMaxValue(double value)
{
    m_maxSpinBox->setValue(value);
}

std::string SubstateDisplayWidget::getFormat() const
{
    return m_formatLineEdit->text().toStdString();
}

void SubstateDisplayWidget::setFormat(const std::string& format)
{
    m_formatLineEdit->setText(QString::fromStdString(format));
}
