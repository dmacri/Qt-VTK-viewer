// sliderconfigurator.h
#ifndef SLIDERCONFIGURATOR_H
#define SLIDERCONFIGURATOR_H

#include <QSlider>

class SliderConfigurator
{
public:
    static void configureSlider(QSlider* slider, const QString& styleSheet);
};

#endif // SLIDERCONFIGURATOR_H
