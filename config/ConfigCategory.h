#pragma once

#include <string>
#include <vector>
#include "ConfigParameter.h"


class ConfigCategory
{
    std::string name;
    std::vector<ConfigParameter> configParameters;

public:
    ConfigCategory(std::string name, std::vector<ConfigParameter> params)
        : name{std::move(name)}, configParameters{std::move(params)}
    {}

    void readFile(const char* /*name*/, const char* /*configuration_path*/)
    {
        #warning "Ask Alessio or Andrea about the function - how it should be implemented, or should it be removed?"
    }

    const std::string& getName() const
    {
        return name;
    }

    auto getSize() const
    {
        return configParameters.size();
    }

    std::vector<ConfigParameter>& getConfigParameters()
    {
        return configParameters;
    }

    const std::vector<ConfigParameter>& getConfigParameters() const
    {
        return configParameters;
    }

    ConfigParameter* getConfigParameter(const std::string& paramName);

    void setConfigParameterValue(const std::string& paramName, const std::string& value);

    bool operator==(const std::string& name) const
    {
        return getName() == name;
    }
};
