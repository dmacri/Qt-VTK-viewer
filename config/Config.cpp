#include <algorithm>
#include <cctype> // isspace()
#include <iostream>
#include <QDebug>
#include <QtGlobal>
#include <stdexcept> // std::runtime_error
#include <string>
#include <fstream>

#include "Config.h"


namespace
{
void remove_spaces(char *s)
{
    if (!s)
        return;

    char* source = s;
    char* destination = s;

    while (*source)
    {
        if (! isspace(static_cast<unsigned char>(*source)))
        {
            *destination++ = *source;
        }
        ++source;
    }
    *destination = '\0';
}

/**
 * @brief Removes all whitespace characters from a std::string.
 *
 * This function erases all characters for which std::isspace() returns true,
 * such as spaces, tabs, newlines, and other locale-defined whitespace.
 * It works in-place and automatically selects a ranges-based implementation
 * if supported by the compiler, otherwise falls back to classic erase–remove idiom.
 *
 * @param s Reference to the std::string to be modified in place.
 */
void remove_spaces(std::string& s)
{
    auto removingRange = std::ranges::remove_if(s, [](unsigned char ch) {
        return std::isspace(ch);
    });
    s.erase(removingRange.begin(), removingRange.end());
}
} // namespace


Config::Config(const std::string& configuration_path)
    : configuration_path{configuration_path}
{
    configCategories.push_back(ConfigCategory{
        "GENERAL",
        {
            {"number_of_columns", "610",  ConfigParameter::int_par},
            {"number_of_rows",    "496",  ConfigParameter::int_par},
            {"number_steps",      "4000", ConfigParameter::int_par},
            {"output_file_name",  "sciddicaTout", ConfigParameter::string_par}
        }
    });
    configCategories.push_back(ConfigCategory{
        "DISTRIBUTED",
        {
            {"border_size_x", "1", ConfigParameter::int_par},
            {"border_size_y", "1", ConfigParameter::int_par},
            {"number_node_x", "4", ConfigParameter::int_par},
            {"number_node_y", "4", ConfigParameter::int_par}
        }
    });

    configCategories.push_back(ConfigCategory{
        "LOAD_BALANCING",
        {
            {"firstLB", "100", ConfigParameter::int_par},
            {"stepLB",  "100", ConfigParameter::int_par}
        }
    });

    // configCategories.push_back(ConfigCategory{
    //     "MPI2DLB2DNaive",
    //     {}
    // });
    // configCategories.push_back(ConfigCategory{
    //     "MPI2DLB2DHierarchical",
    //     {}
    // });
    // configCategories.push_back(ConfigCategory{
    //     "MPI2DSMART",
    //     {}
    // });
    // configCategories.push_back(ConfigCategory{
    //     "CUDA2D",
    //     {}
    // });


    configCategories.push_back(ConfigCategory{
        "MULTICUDA",
        {
            {"number_of_gpus_per_node", "2", ConfigParameter::int_par}
        }
    });

    // configCategories.push_back(ConfigCategory{
    //     "MULTICUDA2DSMART",
    //     {}
    // });

    configCategories.push_back(ConfigCategory{
        "SHARED",
        {
            {"chunk_size", "1", ConfigParameter::int_par}
        }
    });
}

void Config::writeConfigFile() const
{
    std::ofstream file(configuration_path);
    if (! file.is_open())
    {
        throw std::runtime_error(std::string("Can not open file: '") + configuration_path + "' for writing!");
    }

    for (const auto& configCategory : configCategories)
    {
        if (configCategory.getSize() > 0)
        {
            file << configCategory.getName() << ":\n";
            for (const auto& configParameter: configCategory.getConfigParameters())
            {
                file << '\t'
                     << configParameter.getName() << '='
                     << configParameter.getDefaultValue()
                     << '\n';
            }
        }
    }
}

ConfigCategory *Config::getConfigCategory(const std::string& name)
{
    auto it = std::find(begin(configCategories), end(configCategories), name);
    if (it != end(configCategories))
    {
        return &(*it);
    }
    return nullptr;
}

void Config::readConfigFile()
{
    std::cout << std::format("READING '{}'...\n", configuration_path);
    std::ifstream file(configuration_path);
    if (! file.is_open())
    {
        throw std::runtime_error(std::format("Cannot open file '{}' for reading!", configuration_path));
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        if (line.back() != ':')
        {
            throw std::runtime_error("ERROR: Category name must end with the ':' character, line is: " + line);
        }

        line.pop_back(); // removing ':' from the end
        remove_spaces(line);

        ConfigCategory* configCategory = getConfigCategory(line);
        if (! configCategory)
        {
            throw std::runtime_error(std::format("Unknown config category '{}'", line));
        }

        for (int i{}; i < configCategory->getSize(); ++i)
        {
            if (! std::getline(file, line))
            {
                throw std::runtime_error("Unexpected end of file while reading parameters");
            }

            auto pos = line.find('=');
            if (pos == std::string::npos)
            {
                throw std::runtime_error(std::format("Invalid parameter line: '{}'", line));
            }

            std::string parName  = line.substr(0, pos);
            std::string valueStr = line.substr(pos + 1);

            remove_spaces(parName);
            remove_spaces(valueStr);

            configCategory->setConfigParameterValue(parName, valueStr);
        }
    }
}
