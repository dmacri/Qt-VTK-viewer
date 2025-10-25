/** @file ConfigCategory.h
 * @brief Defines the ConfigCategory class for organizing related configuration parameters.
 * 
 * This file contains the ConfigCategory class which represents a named group of
 * related configuration parameters. It provides methods to manage and access
 * these parameters. */

#pragma once

#include <string>
#include <vector>

#include "ConfigParameter.h"

/** @class ConfigCategory
 * @brief Represents a category of related configuration parameters.
 * 
 * A ConfigCategory groups together related configuration parameters under a 
 * common name. It is possible to add params in constructor, then only read or change these parameters.
 * Categories are typically used to organize configuration settings into logical
 * groups (e.g., "GENERAL", "DISTRIBUTED", "LOAD_BALANCING"). */
class ConfigCategory
{
    std::string name;
    std::vector<ConfigParameter> configParameters;

public:
    /** @brief Constructs a new ConfigCategory with the given name and parameters.
     * 
     * @param name The name of the category (e.g., "GENERAL", "DISTRIBUTED")
     * @param params Vector of ConfigParameter objects to include in this category
     *
     * @note: After construction it is impossible to add more config parameters. */
    ConfigCategory(std::string name, std::vector<ConfigParameter> params)
        : name{std::move(name)}
        , configParameters{std::move(params)}
    {
    }

    /// @brief Gets the name of this configuration category.
    const std::string& getName() const
    {
        return name;
    }

    /// @brief Gets the number of parameters in this category.
    auto getSize() const
    {
        return configParameters.size();
    }

    /** @brief Gets a const reference to the vector of configuration parameters.
     * 
     * @return const std::vector<ConfigParameter>& Const reference to the parameters vector */
    const std::vector<ConfigParameter>& getConfigParameters() const
    {
        return configParameters;
    }

    /** @brief Gets a pointer to a configuration parameter by name.
     * 
     * @param paramName The name of the parameter to find
     * @return ConfigParameter* Pointer to the parameter, or nullptr if not found */
    ConfigParameter* getConfigParameter(const std::string& paramName);

    /** @brief Sets the value of a configuration parameter.
     * 
     * @param paramName The name of the parameter to set (the param has to exist already)
     * @param value The new value as a string (will be converted to the parameter's type)
     * 
     * @throws std::invalid_argument If the parameter doesn't exist */
    void setConfigParameterValue(const std::string& paramName, const std::string& value);

    bool operator==(const std::string& name) const
    {
        return getName() == name;
    }
};
