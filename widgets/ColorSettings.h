#pragma once

#include <QColor>
#include <QObject>


class ColorSettings : public QObject
{
    Q_OBJECT

public:
    static ColorSettings& instance();

    // Delete copy constructor and assignment operator
    ColorSettings(const ColorSettings&) = delete;
    ColorSettings& operator=(const ColorSettings&) = delete;

    // Getters
    QColor backgroundColor() const
    {
        return backgroundColor_;
    }
    QColor textColor() const
    {
        return textColor_;
    }
    QColor gridColor() const
    {
        return gridColor_;
    }
    QColor highlightColor() const
    {
        return highlightColor_;
    }

    // Setters
    void setBackgroundColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setGridColor(const QColor& color);
    void setHighlightColor(const QColor& color);

    // Save/load settings
    void saveSettings();
    void loadSettings();

    // Default colors
    static inline const QColor DEFAULT_BACKGROUND{240, 240, 240};
    static inline const QColor DEFAULT_TEXT{Qt::black};
    static inline const QColor DEFAULT_GRID{200, 200, 200};
    static inline const QColor DEFAULT_HIGHLIGHT{65, 105, 225};

signals:
    void colorsChanged();

private:
    ColorSettings(QObject* parent = nullptr);
    ~ColorSettings() = default;

    QColor backgroundColor_;
    QColor textColor_;
    QColor gridColor_;
    QColor highlightColor_;
};
