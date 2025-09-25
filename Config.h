#pragma once

#include "ConfigCategory.h"


class Config
{
    char* configuration_path;
    static constexpr int configCategorySize = 5;
    ConfigCategory* configCategory[configCategorySize];

public:
    Config(char* configuration_path);

    void setConfiguration_path(char* value)
    {
        configuration_path = value;
    }

    void writeConfigFile();

    ConfigCategory* getConfigCategory(const char* name);

    static void remove_spaces(char* s);

    void readConfigFile();
};
