#pragma once

#include "ConfigCategory.h"


class Config
{
    std::string configuration_path;
    std::vector<ConfigCategory> configCategories;

public:
    Config(const std::string& configuration_path);

    void setConfigurationPath(const std::string& value)
    {
        configuration_path = value;
    }

    void writeConfigFile() const;
    void readConfigFile();

    ConfigCategory* getConfigCategory(const std::string &name, bool ignoreCase = false);

    std::vector<std::string> categoryNames() const;

protected:
    void setUpConfigCategories();

    void readConfigFileInOOpenCalFormat();
    void readConfigFileInIniFormat();
};
