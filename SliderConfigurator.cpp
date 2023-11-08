// sliderconfigurator.cpp
#include "SliderConfigurator.h"


void SliderConfigurator::configureSliders(QSlider *slider, int min, int max, int value, const QString &styleSheet)
{
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(value);
    slider->setStyleSheet(styleSheet);
}
