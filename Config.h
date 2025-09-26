#pragma once

#include "ConfigCategory.h"


class Config
{
    const char* configuration_path;
    static constexpr int configCategorySize = 5;
    ConfigCategory* configCategory[configCategorySize];

public:
    Config(const char* configuration_path);

    void setConfiguration_path(char* value)
    {
        configuration_path = value;
    }

    void writeConfigFile() const;
    void readConfigFile();

    ConfigCategory* getConfigCategory(const std::string& name)
    {
        return getConfigCategory(name.c_str());
    }
    ConfigCategory* getConfigCategory(const char* name);
};
