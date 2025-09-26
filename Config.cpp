#include <algorithm>
#include <cctype> // isspace()
#include <iostream>
#include <qdebug.h>
#include <qglobal.h>
#include <stdexcept> // std::runtime_error
#include <string>
#include <fstream>

#include "ConfigParameter.h"
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


Config::Config(const char *configuration_path): configuration_path{configuration_path}
{
    //GENERAL
    const int size2D = 4;
    ConfigParameter** configParameters = new ConfigParameter*[size2D];
    configParameters[0] = new ConfigParameter("number_of_columns", "610"         , ConfigParameter::int_par   );
    configParameters[1] = new ConfigParameter("number_of_rows"   , "496"         , ConfigParameter::int_par   );
    configParameters[2] = new ConfigParameter("number_steps"     , "4000"        , ConfigParameter::int_par   );
    configParameters[3] = new ConfigParameter("output_file_name" , "sciddicaTout", ConfigParameter::string_par);

    ConfigCategory* GENERAL = new ConfigCategory("GENERAL", configParameters, size2D);

    //DISTRIBUTED
    static const int MPI2DSize = 4;
    ConfigParameter** configParameters_MPI2D = new ConfigParameter*[MPI2DSize];
    configParameters_MPI2D[0] = new ConfigParameter("border_size_x", "1", ConfigParameter::int_par);
    configParameters_MPI2D[1] = new ConfigParameter("border_size_y", "1", ConfigParameter::int_par);
    configParameters_MPI2D[2] = new ConfigParameter("number_node_x", "4", ConfigParameter::int_par);
    configParameters_MPI2D[3] = new ConfigParameter("number_node_y", "4", ConfigParameter::int_par);

    ConfigCategory* DISTRIBUTED = new ConfigCategory("DISTRIBUTED", configParameters_MPI2D, MPI2DSize);

    //LOAD_BALANCING
    static const int MPI2DLBSize = 2;
    ConfigParameter** configParameters_MPI2DLB = new ConfigParameter*[MPI2DLBSize];
    configParameters_MPI2DLB[0] = new ConfigParameter("firstLB", "100", ConfigParameter::int_par);
    configParameters_MPI2DLB[1] = new ConfigParameter("stepLB" , "100", ConfigParameter::int_par);
    ConfigCategory* LOAD_BALANCING = new ConfigCategory("LOAD_BALANCING", configParameters_MPI2DLB, MPI2DLBSize);

    // //2D MPI LB2
    // static const int MPI2DLB2DNaiveSize = 0;
    // ConfigCategory*  MPI2DLB2DNaive = new ConfigCategory("MPI2DLB2DNaive", NULL, MPI2DLB2DNaiveSize);

    // //2D MPI LB3
    // static const int MPI2DLB2DHierarchicalSize = 0;
    // ConfigCategory* MPI2DLB2DHierarchical = new ConfigCategory("MPI2DLB2DHierarchical", NULL, MPI2DLB2DHierarchicalSize);

    // //2D MPI SMART
    // static const int MPI2DSMARTSize = 0;
    // ConfigCategory* MPI2DSMART = new ConfigCategory("MPI2DSMART", NULL, MPI2DSMARTSize);

    // //2D CUDA
    // static const int CUDA2DSize = 0;
    // ConfigCategory* CUDA2D = new ConfigCategory("CUDA2D", NULL, CUDA2DSize);

    //MULTICUDA
    static const int MULTICUDA2DSize = 1;
    ConfigParameter** configParameters_MULTICUDA2D = new ConfigParameter*[MULTICUDA2DSize];
    configParameters_MULTICUDA2D[0] = new ConfigParameter("number_of_gpus_per_node", "2", ConfigParameter::int_par   );

    ConfigCategory* MULTICUDA = new ConfigCategory("MULTICUDA", configParameters_MULTICUDA2D, MULTICUDA2DSize);

    // //2D MULTICUDASMART
    // static const int MULTICUDA2DSMARTSize = 0;
    // ConfigCategory* MULTICUDA2DSMART = new ConfigCategory("MULTICUDA2DSMART", NULL, MULTICUDA2DSMARTSize);

    //SHARED
    static const int SHAREDSize = 1;
    ConfigParameter** configParameters_SHARED = new ConfigParameter*[SHAREDSize];
    configParameters_SHARED[0] = new ConfigParameter("chunk_size", "1", ConfigParameter::int_par   );

    ConfigCategory* SHARED = new ConfigCategory("SHARED", configParameters_SHARED, SHAREDSize);

    configCategory[0] = GENERAL;
    configCategory[1] = DISTRIBUTED;
    configCategory[2] = LOAD_BALANCING;
    // configCategory[3] = MPI2DLB2DNaive;
    // configCategory[4] = MPI2DLB2DHierarchical;
    // configCategory[5] = MPI2DSMART;
    // configCategory[6] = CUDA2D;
    configCategory[3] = MULTICUDA;
    // configCategory[8] = MULTICUDA2DSMART;
    configCategory[4] = SHARED;
}

void Config::writeConfigFile() const
{
    std::ofstream file(configuration_path);
    if (! file.is_open())
    {
        throw std::runtime_error(std::string("Can not open file: '") + configuration_path + "' for writing!");
    }

    for (int i = 0; i < configCategorySize; ++i)
    {
        if (configCategory[i]->getSize() > 0)
        {
            file << configCategory[i]->getName() << ":\n";
            for (int p = 0; p < configCategory[i]->getSize(); ++p)
            {
                auto* configParameter = configCategory[i]->getConfigParameters()[p];
                file << '\t'
                     << configParameter->getName() << '='
                     << configParameter->getDefaultValue()
                     << '\n';
            }
        }
    }
}

ConfigCategory *Config::getConfigCategory(const char *name)
{
    for(int i = 0; i < configCategorySize; i++)
    {
        if(strcmp(name, configCategory[i]->getName()) == 0)
            return configCategory[i];
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
        if (!configCategory)
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

            char* parName_cstr = new char[parName.size() + 1]{};
            parName.copy(parName_cstr, parName.size() + 1);

            char* valueStr_cstr = new char[valueStr.size() + 1]{};
            valueStr.copy(valueStr_cstr, valueStr.size() + 1);


            configCategory->setConfigParameterValue(parName_cstr, valueStr_cstr);
        }
    }
}
