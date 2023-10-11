// sliderconfigurator.cpp
#include "SliderConfigurator.h"


void SliderConfigurator::configureSlider(QSlider* slider, const QString& styleSheet)
{
    slider->setMinimum(0);
    slider->setMaximum(100);
    slider->setValue(50);
    slider->setStyleSheet(styleSheet);
}
