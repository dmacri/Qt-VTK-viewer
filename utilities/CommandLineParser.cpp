#include "CommandLineParser.h"
#include <argparse/argparse.hpp>
#include <iostream>

bool CommandLineParser::parse(int argc, char* argv[])
{
    try
    {
        argparse::ArgumentParser program("Visualiser");
        program.add_description("Visualizer for simulation data with VTK and Qt");
        program.add_epilog("Examples:\n"
                          "  Visualiser config.txt\n"
                          "  Visualiser config.txt --startingModel=MyModel\n"
                          "  Visualiser --generateMoviePath=/tmp/movie --exitAfterLastStep");

        // Positional argument: configuration file
        program.add_argument("config")
            .help("Path to configuration file")
            .nargs(argparse::nargs_pattern::optional);

        // Optional arguments
        program.add_argument("--loadModel")
            .help("Path to custom model plugin (can be repeated)")
            .append();

        program.add_argument("--startingModel")
            .help("Name of the model to start with");

        program.add_argument("--generateMoviePath")
            .help("Generate movie by running all steps (for testing)");

        program.add_argument("--generateImagePath")
            .help("Generate image for current step and save to file");

        program.add_argument("--step")
            .help("Go to specific step directly")
            .scan<'i', int>();

        program.add_argument("--exitAfterLastStep")
            .help("Exit after last step (useful with --generateMoviePath)")
            .flag();

        try
        {
            program.parse_args(argc, argv);
        }
        catch (const std::runtime_error& err)
        {
            std::cerr << err.what() << std::endl;
            std::cerr << program << std::endl;
            return false;
        }

        // Parse positional config file argument
        try
        {
            auto configArg = program.get<std::vector<std::string>>("config");
            if (! configArg.empty())
            {
                configFile = configArg[0];
            }
        }
        catch (const std::logic_error&)
        {
            // config argument not provided, which is fine
        }

        // Parse loadModel (can be repeated)
        try
        {
            loadModelPaths = program.get<std::vector<std::string>>("--loadModel");
        }
        catch (const std::logic_error&)
        {
            // --loadModel not provided
        }

        // Parse startingModel
        try
        {
            startingModel = program.get<std::string>("--startingModel");
        }
        catch (const std::logic_error&)
        {
            // --startingModel not provided
        }

        // Parse generateMoviePath
        try
        {
            generateMoviePath = program.get<std::string>("--generateMoviePath");
        }
        catch (const std::logic_error&)
        {
            // --generateMoviePath not provided
        }

        // Parse generateImagePath
        try
        {
            generateImagePath = program.get<std::string>("--generateImagePath");
        }
        catch (const std::logic_error&)
        {
            // --generateImagePath not provided
        }

        // Parse step
        try
        {
            step = program.get<int>("--step");
        }
        catch (const std::logic_error&)
        {
            // --step not provided
        }

        // Parse exitAfterLastStep flag
        try
        {
            exitAfterLastStep = program.get<bool>("--exitAfterLastStep");
        }
        catch (const std::logic_error&)
        {
            // --exitAfterLastStep not provided
        }

        return true;
    }
    catch (const std::exception& err)
    {
        std::cerr << "Error parsing command-line arguments: " << err.what() << std::endl;
        printHelp();
        return false;
    }
}

void CommandLineParser::printHelp() const
{
    std::cout << "Usage: Visualiser [CONFIG_FILE] [OPTIONS]\n\n"
              << "Positional Arguments:\n"
              << "  CONFIG_FILE              Path to configuration file (optional)\n\n"
              << "Optional Arguments:\n"
              << "  --loadModel PATH         Load custom model plugin (can be repeated)\n"
              << "  --startingModel NAME     Start with specific model\n"
              << "  --generateMoviePath PATH Generate movie by running all steps\n"
              << "  --generateImagePath PATH Generate image for current step\n"
              << "  --step NUMBER            Go to specific step directly\n"
              << "  --exitAfterLastStep      Exit after last step\n"
              << "  -h, --help               Show this help message\n\n"
              << "Examples:\n"
              << "  Visualiser config.txt\n"
              << "  Visualiser config.txt --startingModel=MyModel\n"
              << "  Visualiser --generateMoviePath=/tmp/movie --exitAfterLastStep\n";
}
