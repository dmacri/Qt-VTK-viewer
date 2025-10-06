#include <algorithm>
#include "ConfigCategory.h"


ConfigParameter *ConfigCategory::getConfigParameter(const std::string &paramName)
{
    auto it = std::find_if(configParameters.begin(), configParameters.end(),
                           [&](const ConfigParameter& p) { return p.getName() == paramName; });
    return (it != configParameters.end()) ? &*it : nullptr;
}

void ConfigCategory::setConfigParameterValue(const std::string &paramName, const std::string &value)
{
    if (auto* configParameter = getConfigParameter(paramName))
    {
        configParameter->setDefaultValue(value);
    }
}
