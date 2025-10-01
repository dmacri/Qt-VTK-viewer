#pragma once

#include "ConfigCategory.h"


class Config
{
    std::string configuration_path;
    std::vector<ConfigCategory> configCategories;

public:
    Config(const std::string& configuration_path);

    void setConfiguration_path(const std::string& value)
    {
        configuration_path = value;
    }

    void writeConfigFile() const;
    void readConfigFile();

    ConfigCategory* getConfigCategory(const std::string &name);

    std::vector<std::string> categoryNames() const;
};
