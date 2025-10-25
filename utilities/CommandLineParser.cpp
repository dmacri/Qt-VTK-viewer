#include <iostream>
#include <argparse/argparse.hpp>
#include <QApplication>
#include "CommandLineParser.h"


bool CommandLineParser::parse(int argc, char* argv[])
{
    const auto appName = QApplication::applicationName().toStdString();
    try
    {
        argparse::ArgumentParser program(appName);
        program.add_description(appName + " for simulation data with VTK and Qt");
        program.add_epilog("Examples:\n" +
                           std::format("  {} config.txt\n", appName) +
                           std::format("  {} config.txt {}=MyModel\n", appName, ARG_STARTING_MODEL) +
                           std::format("  {} {}=/tmp/movie {}", appName, ARG_GENERATE_MOVIE, ARG_EXIT_AFTER_LAST));

        // Positional argument: configuration file
        program.add_argument(ARG_CONFIG)
            .help("Path to configuration file")
            .nargs(argparse::nargs_pattern::optional);

        // Optional arguments
        program.add_argument(ARG_LOAD_MODEL)
            .help("Path to custom model plugin (can be repeated)")
            .append();

        program.add_argument(ARG_STARTING_MODEL)
            .help("Name of the model to start with");

        program.add_argument(ARG_GENERATE_MOVIE)
            .help("Generate movie by running all steps (for testing)");

        program.add_argument(ARG_GENERATE_IMAGE)
            .help("Generate image for current step and save to file");

        program.add_argument(ARG_STEP)
            .help("Go to specific step directly")
            .scan<'i', int>();

        program.add_argument(ARG_EXIT_AFTER_LAST)
            .help("Exit after last step (useful with --generateMoviePath)")
            .flag();

        program.add_argument(ARG_SILENT)
            .help("Silent mode: Skip displaying information dialogs (usefull when we use commandline arguments)")
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
            auto configArg = program.get<std::vector<std::string>>(ARG_CONFIG);
            if (! configArg.empty())
            {
                configFile = configArg[0];
            }
        }
        catch (const std::logic_error&)
        {
            // argument not provided, which is fine
        }

        try
        {
            loadModelPaths = program.get<std::vector<std::string>>(ARG_LOAD_MODEL);
        }
        catch (const std::logic_error&)
        {
        }

        try
        {
            startingModel = program.get<std::string>(ARG_STARTING_MODEL);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
        }

        // Parse generateMoviePath
        try
        {
            generateMoviePath = program.get<std::string>(ARG_GENERATE_MOVIE);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
        }

        try
        {
            generateImagePath = program.get<std::string>(ARG_GENERATE_IMAGE);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
        }

        try
        {
            step = program.get<int>(ARG_STEP);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
        }

        try
        {
            exitAfterLastStep = program.get<bool>(ARG_EXIT_AFTER_LAST);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
        }

        try
        {
            silentMode = program.get<bool>(ARG_SILENT);
        }
        catch (const std::logic_error&)
        {
            // argument not provided
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
    constexpr int WIDTH = 24;
    const auto appName = QApplication::applicationName().toStdString();
    std::cout << std::format("Usage: {} [CONFIG_FILE] [OPTIONS]\n\n", appName)
              << "Positional Arguments:\n"
              << std::format("  {: <{}} Path to configuration file (optional)\n\n", "CONFIG_FILE", WIDTH)
              << "Optional Arguments:\n"
              << std::format("  {: <{}} Load custom model plugin (can be repeated)\n", ARG_LOAD_MODEL, WIDTH)
              << std::format("  {: <{}} Start with specific model\n", ARG_STARTING_MODEL, WIDTH)
              << std::format("  {: <{}} Generate movie by running all steps\n", ARG_GENERATE_MOVIE, WIDTH)
              << std::format("  {: <{}} Generate image for current step\n", ARG_GENERATE_IMAGE, WIDTH)
              << std::format("  {: <{}} Go to specific step directly\n", ARG_STEP, WIDTH)
              << std::format("  {: <{}} Exit after last step\n", ARG_EXIT_AFTER_LAST, WIDTH)
              << std::format("  {: <{}} Suppress error dialogs and messages\n", ARG_SILENT, WIDTH)
              << std::format("  {: <{}} Show this help message\n\n", "-h, --help", WIDTH)
              << "Examples:\n"
              << std::format("  {} config.txt\n", appName)
              << std::format("  {} config.txt {}=MyModel\n", appName, ARG_STARTING_MODEL)
              << std::format("  {} {}=/tmp/movie {}\n", appName, ARG_GENERATE_MOVIE, ARG_EXIT_AFTER_LAST);
}
