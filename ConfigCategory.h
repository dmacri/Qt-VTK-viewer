#pragma once

#include <cstring>
#include "ConfigParameter.h"


class ConfigCategory
{
    const char* name;
    ConfigParameter** configParameters;
    int size;

public:
    ConfigCategory(const char* name, ConfigParameter** configParameters, const int& size)
    {
        this->name             = name;
        this->configParameters = configParameters;
        this->size             = size;
    }

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

    void setConfigParameterValue(const char* name, const char* value)
    {
        for(int i = 0; i < size; i++)
        {
            //   printf("Controllo |%s| == |%s| \n ", name, configParameters[i]->getName());
            if(strcmp(name, configParameters[i]->getName()) == 0)
            {
                //  printf("Sono uguali %s == %s \n ", name, configParameters[i]->getName());
                //  printf("inserisco %s \n ", value);
                configParameters[i]->setDefaultValue(value);
                //  printf("getDefaultValue ho inserito %s \n ", configParameters[i]->getDefaultValue());
                //  printf("getValue ho inserito %d \n ", configParameters[i]->getValue());

            }
        }
    }

    ConfigParameter* getConfigParameter(const char* name)
    {
        for(int i = 0; i < size; i++)
        {
            if(strcmp(name, configParameters[i]->getName()) == 0)
            {
                return configParameters[i];
            }
        }
        return nullptr;
    }
};
