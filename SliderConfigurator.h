// sliderconfigurator.h
#ifndef SLIDERCONFIGURATOR_H
#define SLIDERCONFIGURATOR_H

#include <QSlider>

class SliderConfigurator
{
public:
    static void configureSliders(QSlider *slider, int min, int max, int value, const QString &styleSheet);
};

#endif // SLIDERCONFIGURATOR_H
