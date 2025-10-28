/** @file ConfigParameter.h
 * @brief Defines the ConfigParameter class for managing individual configuration settings.
 * 
 * This file contains the ConfigParameter class which represents a single configuration
 * parameter with a name, value, and type. It supports different parameter types
 * including integers, doubles, and strings. */

#pragma once

#include <stdexcept> // std::runtime_error
#include <string>

/** @class ConfigParameter
 * @brief Represents a single configuration parameter with a name, value, and type.
 * 
 * The ConfigParameter class encapsulates a configuration setting with methods to
 * get and set its value while maintaining type safety. It supports the following
 * parameter types:
 *   - Integer (int_par)
 *   - Double (double_par)
 *   - String (string_par) */
class ConfigParameter
{
    const std::string name;
    std::string defaultValue;
    const int type;

public:
    /// @brief Enumerates the supported parameter types.
    enum ParamType
    {
        int_par = 0,    ///< Integer parameter type
        double_par = 1, ///< Double-precision floating point parameter type
        string_par = 2  ///< String parameter type
    };

    /** @brief Constructs a new ConfigParameter with the given properties.
     * 
     * @param name The name of the parameter
     * @param defaultValue The default value as a string (will be converted to the specified type)
     * @param type The parameter type (one of ParamType values)
     * 
     * @note The defaultValue string will be converted to the specified type when retrieved. */
    ConfigParameter(const std::string& name, const std::string& defaultValue, int type)
        : name{ name }
        , defaultValue{ defaultValue }
        , type{ type }
    {
    }

    /** @brief Gets the parameter value converted to the specified type.
     * 
     * @tparam RetVal The type to convert the parameter value to (int, double, or std::string)
     * @return RetVal The parameter value converted to the requested type
     * 
     * @throws std::invalid_argument If the conversion fails
     * 
     * @note This is a template method with specializations for int, double, and std::string */
    template<typename RetVal>
    RetVal getValue() const;

    /// @brief Gets the name of this parameter.
    const auto& getName() const
    {
        return name;
    }

    /// @brief Gets the default value of this parameter as a string.
    const auto& getDefaultValue() const
    {
        return defaultValue;
    }

    /** @brief Sets the default value of this parameter.
     * 
     * @param value The new default value as a string
     * 
     * @note The string will be converted to the parameter's type when retrieved. */
    void setDefaultValue(const std::string& value)
    {
        defaultValue = value;
    }

    /** @brief Gets the type of this parameter.
     * 
     * @return int The parameter type (one of ParamType values) */
    int getType() const
    {
        return type;
    }
};

template<>
inline int ConfigParameter::getValue() const
{
    if (int_par == type)
        return std::stoi(defaultValue);

    throw std::runtime_error("Type does not match!");
}

template<>
inline std::string ConfigParameter::getValue() const
{
    if (string_par == type)
        return defaultValue;

    throw std::runtime_error("Type does not match!");
}
