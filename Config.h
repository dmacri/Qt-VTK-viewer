#ifndef CONFIG_H
#define CONFIG_H
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;

class ConfigParameter{

private:
    char* name;
    char* defaultValue;
    int type;

public:
    static const int int_par    = 0;
    static const int double_par = 1;
    static const int string_par = 2;

    ConfigParameter(char* name, char* defaultValue, int type)
    {
        this->name         = name;
        this->defaultValue = defaultValue;
        this->type         = type;
    }

    void* getValue()
    {
        switch (type)
        {
        case int_par:
            return (void*) (atoi(defaultValue));
            break;

            // case double_par:
            //     return (void*) (atof(defaultValue));
            //     break;

        case string_par:
            return (void*) (defaultValue);
            break;

        default:
            break;
        }
    }

    char* getName()
    {
        return name;
    }

    char* getDefaultValue()
    {
        return defaultValue;
    }

    void setDefaultValue(char* value)
    {
        defaultValue = value;
    }

    int getType()
    {
        return type;
    }
};

class ExecutionContext{

private:
    char* name;
    ConfigParameter** configParameters;
    int size;

public:
    ExecutionContext(char* name, ConfigParameter** configParameters, const int& size)
    {
        this->name             = name;
        this->configParameters = configParameters;
        this->size             = size;
    }

    void readFile(char* name, char* configuration_path)
    {
        //...
    }

    char* getName()
    {
        return name;
    }

    int getSize()
    {
        return size;
    }

    ConfigParameter** getConfigParameters()
    {
        return configParameters;
    }

    void setConfigParameterValue(char* name, char* value)
    {
        for(int i = 0; i < size; i++)
        {
            //   printf("Controllo |%s| == |%s| \n ", name, configParameters[i]->getName());
            if(strcmp(name, configParameters[i]->getName()) == 0)
            {
                //  printf("Sono uguali %s == %s \n ", name, configParameters[i]->getName());
                //  printf("inserisco %s \n ", value);
                configParameters[i]->setDefaultValue(value);
                //  printf("getDefaultValue ho inserito %s \n ", configParameters[i]->getDefaultValue());
                //  printf("getValue ho inserito %d \n ", configParameters[i]->getValue());

            }
        }
    }

    ConfigParameter* getConfigParameter(char* name)
    {
        for(int i = 0; i < size; i++)
        {
            if(strcmp(name, configParameters[i]->getName()) == 0)
            {
                return configParameters[i];
            }
        }
    }
};

class Config{

private:
    char* configuration_path;
    static const int executionContextSize = 9;
    ExecutionContext* executionContext[executionContextSize];

public:
    Config( char* configuration_path)
    {
        this->configuration_path =  configuration_path;

        //2D
        const int size2D = 4;
        ConfigParameter** configParameters = new ConfigParameter*[size2D];
        configParameters[0] = new ConfigParameter("number_of_columns", "610"          , ConfigParameter::int_par   );
        configParameters[1] = new ConfigParameter("number_of_rows"   , "496"          , ConfigParameter::int_par   );
        configParameters[2] = new ConfigParameter("number_steps"     , "4000"         , ConfigParameter::int_par   );
        configParameters[3] = new ConfigParameter("output_file_name" , "sciddicaTout" , ConfigParameter::string_par);

        ExecutionContext* general = new ExecutionContext("general", configParameters, size2D);

        //2D MPI
        static const int MPI2DSize = 4;
        ConfigParameter** configParameters_MPI2D = new ConfigParameter*[MPI2DSize];
        configParameters_MPI2D[0] = new ConfigParameter("border_size_x", "1", ConfigParameter::int_par   );
        configParameters_MPI2D[1] = new ConfigParameter("border_size_y", "1", ConfigParameter::int_par   );
        configParameters_MPI2D[2] = new ConfigParameter("number_node_x", "4", ConfigParameter::int_par   );
        configParameters_MPI2D[3] = new ConfigParameter("number_node_y", "4", ConfigParameter::int_par);

        ExecutionContext* MPI2D = new ExecutionContext("MPI2D", configParameters_MPI2D, MPI2DSize);

        //2D MPI LB
        static const int MPI2DLBSize = 2;
        ConfigParameter** configParameters_MPI2DLB = new ConfigParameter*[MPI2DLBSize];
        configParameters_MPI2DLB[0] = new ConfigParameter("firstLB", "100", ConfigParameter::int_par   );
        configParameters_MPI2DLB[1] = new ConfigParameter("stepLB" , "100", ConfigParameter::int_par   );
        ExecutionContext* MPI2DLB = new ExecutionContext("MPI2DLB", configParameters_MPI2DLB, MPI2DLBSize);

        //2D MPI LB2
        static const int MPI2DLB2DNaiveSize = 0;
        ExecutionContext*  MPI2DLB2DNaive = new ExecutionContext("MPI2DLB2DNaive", NULL, MPI2DLB2DNaiveSize);

        //2D MPI LB3
        static const int MPI2DLB2DHierarchicalSize = 0;
        ExecutionContext* MPI2DLB2DHierarchical = new ExecutionContext("MPI2DLB2DHierarchical", NULL, MPI2DLB2DHierarchicalSize);

        //2D MPI SMART
        static const int MPI2DSMARTSize = 0;
        ExecutionContext* MPI2DSMART = new ExecutionContext("MPI2DSMART", NULL, MPI2DSMARTSize);

        //2D CUDA
        static const int CUDA2DSize = 0;
        ExecutionContext* CUDA2D = new ExecutionContext("CUDA2D", NULL, CUDA2DSize);

        //2D MULTICUDA
        static const int MULTICUDA2DSize = 1;
        ConfigParameter** configParameters_MULTICUDA2D = new ConfigParameter*[MULTICUDA2DSize];
        configParameters_MULTICUDA2D[0] = new ConfigParameter("number_of_gpus_per_node", "2", ConfigParameter::int_par   );

        ExecutionContext* MULTICUDA2D = new ExecutionContext("MULTICUDA2D", configParameters_MULTICUDA2D, MULTICUDA2DSize);

        //2D MULTICUDASMART
        static const int MULTICUDA2DSMARTSize = 0;
        ExecutionContext* MULTICUDA2DSMART = new ExecutionContext("MULTICUDA2DSMART", NULL, MULTICUDA2DSMARTSize);


        executionContext[0] = general;
        executionContext[1] = MPI2D;
        executionContext[2] = MPI2DLB;
        executionContext[3] = MPI2DLB2DNaive;
        executionContext[4] = MPI2DLB2DHierarchical;
        executionContext[5] = MPI2DSMART;
        executionContext[6] = CUDA2D;
        executionContext[7] = MULTICUDA2D;
        executionContext[8] = MULTICUDA2DSMART;

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
        for(int i = 0; i < executionContextSize; i++)
        {
            if (executionContext[i]->getSize() > 0)
            {
                fprintf(fptr, "%s:\n", executionContext[i]->getName() );
                // cout <<  executionContext[i]->getSize() << endl;
                for(int p = 0; p <  executionContext[i]->getSize(); p++)
                {
                    //    cout << executionContext[i]->getConfigParameters()[p]->getName() << " " << executionContext[i]->getConfigParameters()[p]->getDefaultValue() << endl;
                    fprintf(fptr, "\t%s=%s\n", executionContext[i]->getConfigParameters()[p]->getName(), executionContext[i]->getConfigParameters()[p]->getDefaultValue());
                }
            }
        }
        fclose(fptr);
    }

    ExecutionContext* getExecutionContext(char* name)
    {
        for(int i = 0; i < executionContextSize; i++)
        {
            if(strcmp(name, executionContext[i]->getName()) == 0)
                return  executionContext[i];
        }
        return NULL;
    }

    void remove_spaces(char* s) {
        char* d = s;
        do {
            while (*d == ' ' || *d == '\t' || *d == '\n') {
                ++d;
            }
        } while (*s++ = *d++);
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
                printf("ERROR: first line must have : ");
                exit(EXIT_FAILURE);
            }else
            {
                char* contName = new char[read-1];
                for (int i = 0; i < read-1; ++i) {
                    contName[i]=0;
                }
                strncpy(contName, line, read-2);
                ExecutionContext* executionContext = getExecutionContext(contName);
                // printf("getContextName %s \n", executionContext->getName());
                // printf("getContextSize %d \n", executionContext->getSize());
                // printf("\n");
                for(int i = 0; i < executionContext->getSize(); i++)
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

                    executionContext->setConfigParameterValue(parName, vStr);
                }
                // printf("\n");
            }
            //printf("Retrieved line of length %d:\n", read);
            //printf("%s", line);
        }
        fclose(fptr);
    }

};






#endif
