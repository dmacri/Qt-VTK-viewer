#pragma once

#include <cstring>
#include "ConfigParameter.h"


class ConfigCategory
{
    const char* name;
    ConfigParameter** configParameters;
    int size;

public:
    ConfigCategory(const char* name, ConfigParameter** configParameters, int size)
        : name{name}, configParameters{configParameters}, size{size}
    {}

    void readFile(char* name, char* configuration_path)
    {
        #warning "Ask Alessio or Andrea about the function - how it should be implemented, or should it be removed?"
    }

    const char* getName() const
    {
        return name;
    }

    int getSize() const
    {
        return size;
    }

    ConfigParameter** getConfigParameters()
    {
        return configParameters;
    }

    ConfigParameter* getConfigParameter(const char* name)
    {
        for(int i = 0; i < size; i++)
        {
            if(configParameters[i]->getName() == name)
            {
                return configParameters[i];
            }
        }
        return nullptr;
    }

    void setConfigParameterValue(const char* name, const char* value)
    {
        ConfigParameter* configParameter = getConfigParameter(name);
        if (configParameter)
            configParameter->setDefaultValue(value);
    }
};
