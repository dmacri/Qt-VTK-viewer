#pragma once

#include <cctype> // isspace()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ConfigParameter.h"
#include "ConfigCategory.h"

using namespace std;

class Config
{
    char* configuration_path;
    static const int configCategorySize = 5;
    ConfigCategory* configCategory[configCategorySize];

public:
    Config(char* configuration_path)
    {
        this->configuration_path =  configuration_path;

        //GENERAL
        const int size2D = 4;
        ConfigParameter** configParameters = new ConfigParameter*[size2D];
        configParameters[0] = new ConfigParameter("number_of_columns", "610"          , ConfigParameter::int_par   );
        configParameters[1] = new ConfigParameter("number_of_rows"   , "496"          , ConfigParameter::int_par   );
        configParameters[2] = new ConfigParameter("number_steps"     , "4000"         , ConfigParameter::int_par   );
        configParameters[3] = new ConfigParameter("output_file_name" , "sciddicaTout" , ConfigParameter::string_par);

        ConfigCategory* GENERAL = new ConfigCategory("GENERAL", configParameters, size2D);

        //DISTRIBUTED
        static const int MPI2DSize = 4;
        ConfigParameter** configParameters_MPI2D = new ConfigParameter*[MPI2DSize];
        configParameters_MPI2D[0] = new ConfigParameter("border_size_x", "1", ConfigParameter::int_par   );
        configParameters_MPI2D[1] = new ConfigParameter("border_size_y", "1", ConfigParameter::int_par   );
        configParameters_MPI2D[2] = new ConfigParameter("number_node_x", "4", ConfigParameter::int_par   );
        configParameters_MPI2D[3] = new ConfigParameter("number_node_y", "4", ConfigParameter::int_par);

        ConfigCategory* DISTRIBUTED = new ConfigCategory("DISTRIBUTED", configParameters_MPI2D, MPI2DSize);

        //LOAD_BALANCING
        static const int MPI2DLBSize = 2;
        ConfigParameter** configParameters_MPI2DLB = new ConfigParameter*[MPI2DLBSize];
        configParameters_MPI2DLB[0] = new ConfigParameter("firstLB", "100", ConfigParameter::int_par   );
        configParameters_MPI2DLB[1] = new ConfigParameter("stepLB" , "100", ConfigParameter::int_par   );
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

    void setConfiguration_path(char* value)
    {
        configuration_path = value;
    }

    void writeConfigFile()
    {
        FILE *fptr;

        fptr = fopen(configuration_path, "w");
        // cout <<  executionContextSize << endl;
        for(int i = 0; i < configCategorySize; i++)
        {
            if (configCategory[i]->getSize() > 0)
            {
                fprintf(fptr, "%s:\n", configCategory[i]->getName() );
                // cout <<  executionContext[i]->getSize() << endl;
                for(int p = 0; p <  configCategory[i]->getSize(); p++)
                {
                    //    cout << executionContext[i]->getConfigParameters()[p]->getName() << " " << executionContext[i]->getConfigParameters()[p]->getDefaultValue() << endl;
                    fprintf(fptr, "\t%s=%s\n", configCategory[i]->getConfigParameters()[p]->getName(), configCategory[i]->getConfigParameters()[p]->getDefaultValue());
                }
            }
        }
        fclose(fptr);
    }

    ConfigCategory* getConfigCategory(const char* name)
    {
        for(int i = 0; i < configCategorySize; i++)
        {
            if(strcmp(name, configCategory[i]->getName()) == 0)
                return  configCategory[i];
        }
        return NULL;
    }

    void remove_spaces(char* s)
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

    void readConfigFile()
    {
        printf("READING...\n");
        FILE *fptr;
        fptr = fopen(configuration_path, "r");
        if (fptr == NULL)
        {
            printf("%s does not exist", configuration_path);
            exit(EXIT_FAILURE);
        }
        char * line = NULL;
        size_t len = 0;
        int read;
        bool head = true;

        while ((read = getline(&line, &len, fptr)) != -1) {
            if(line[read-2] != ':')
            {
                printf("ERROR: Category name must end with the ':' character ");
                exit(EXIT_FAILURE);
            }else
            {

                char* contName = new char[read-1];
                strncpy(contName, line, read-2);
                contName[read-2]=0;
                ConfigCategory* configCategory = getConfigCategory(contName);
                // printf("getContextName %s \n", executionContext->getName());
                // printf("getContextSize %d \n", executionContext->getSize());
                // printf("\n");
                for(int i = 0; i < configCategory->getSize(); i++)
                {
                    read = getline(&line, &len, fptr);
                    char* parName  = strtok (line,"=");
                    char* valueStr = strtok (NULL,"=");
                    remove_spaces(parName);
                    remove_spaces(valueStr);
                    char* vStr = new char[strlen(valueStr)];
                    strcpy(vStr, valueStr);
                    // printf("parName |%s| \n", parName);
                    // printf("valueStr |%s| \n", vStr);

                    configCategory->setConfigParameterValue(parName, vStr);
                }
                // printf("\n");
            }
            //printf("Retrieved line of length %d:\n", read);
            //printf("%s", line);
        }
        fclose(fptr);
    }
};
