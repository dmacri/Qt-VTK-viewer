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
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Title with field name
    auto titleLayout = new QHBoxLayout();
    auto titleLabel = new QLabel("Param:");
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(m_nameLabel);
    titleLayout->addStretch();
    mainLayout->addLayout(titleLayout);

    // Value display (read-only)
    auto valueLayout = new QHBoxLayout();
    auto valueTextLabel = new QLabel("Value:");
    m_valueLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 2px; }");
    valueLayout->addWidget(valueTextLabel);
    valueLayout->addWidget(m_valueLabel);
    mainLayout->addLayout(valueLayout);

    // Minimum value
    auto minLayout = new QHBoxLayout();
    auto minTextLabel = new QLabel("Min:");
    m_minSpinBox->setRange(-1e9, 1e9);
    m_minSpinBox->setDecimals(2);
    m_minSpinBox->setValue(0.0);
    minLayout->addWidget(minTextLabel);
    minLayout->addWidget(m_minSpinBox);
    mainLayout->addLayout(minLayout);

    // Maximum value
    auto maxLayout = new QHBoxLayout();
    auto maxTextLabel = new QLabel("Max:");
    m_maxSpinBox->setRange(-1e9, 1e9);
    m_maxSpinBox->setDecimals(2);
    m_maxSpinBox->setValue(0.0);
    maxLayout->addWidget(maxTextLabel);
    maxLayout->addWidget(m_maxSpinBox);
    mainLayout->addLayout(maxLayout);

    // Format string
    auto formatLayout = new QHBoxLayout();
    auto formatTextLabel = new QLabel("Format:");
    formatLayout->addWidget(formatTextLabel);
    formatLayout->addWidget(m_formatLineEdit);
    mainLayout->addLayout(formatLayout);

    // Use as 3rd dimension button
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
