#pragma once

#include <string>
#include <stdexcept> // std::runtime_error


class ConfigParameter
{
    const std::string name;
    const char* defaultValue;
    const int type;

public:
    enum ParamType
    {
        int_par    = 0,
        double_par = 1,
        string_par = 2
    };

    ConfigParameter(const std::string& name, const char* defaultValue, int type)
        : name{name}, defaultValue{defaultValue}, type{type}
    {}

    template<typename RetVal>
    RetVal getValue() const;

    const auto& getName() const
    {
        return name;
    }

    const char* getDefaultValue()
    {
        return defaultValue;
    }

    void setDefaultValue(const char* value)
    {
        defaultValue = value;
    }

    int getType() const
    {
        return type;
    }

    bool operator<(const ConfigParameter& configParameter) const
    {
        return name < configParameter.name;
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
inline const char* ConfigParameter::getValue() const
{
    if (string_par == type)
        return defaultValue;

    throw std::runtime_error("Type does not match!");
}
