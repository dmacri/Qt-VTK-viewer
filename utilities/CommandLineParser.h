/** @file CommandLineParser.h
 * @brief Command-line argument parser for the Visualizer application.
 *
 * This class handles parsing and storing command-line arguments used to configure
 * the Visualizer at startup. It supports loading custom models, setting initial
 * model/step, and generating images/movies for testing purposes. */

#pragma once

#include <string>
#include <vector>
#include <optional>

/** @class CommandLineParser
 * @brief Parses and stores command-line arguments for the Visualizer.
 *
 * Supported arguments:
 * - loadModel=<path>: Load a custom model plugin (can be repeated)
 * - startingModel=<name>: Start with specific model
 * - generateMoviePath=<path>: Generate movie by running all steps (testing)
 * - exitAfterLastStep: Exit after last step (useful with generateMoviePath)
 * - step=<number>: Go to specific step directly
 * - generateImagePath=<path>: Generate image for current step and save to file
 * - configFile: Path to configuration file (positional argument) */
class CommandLineParser
{
public:
    /** @brief Parse command-line arguments.
     * @param argc Number of arguments
     * @param argv Array of argument strings
     * @return true if parsing succeeded, false otherwise */
    bool parse(int argc, char* argv[]);

    // Getters for parsed arguments
    const std::vector<std::string>& getLoadModelPaths() const
    {
        return loadModelPaths;
    }
    const std::optional<std::string>& getStartingModel() const
    {
        return startingModel;
    }
    const std::optional<std::string>& getGenerateMoviePath() const
    {
        return generateMoviePath;
    }
    const std::optional<std::string>& getGenerateImagePath() const
    {
        return generateImagePath;
    }
    const std::optional<int>& getStep() const
    {
        return step;
    }
    const std::optional<std::string>& getConfigFile() const
    {
        return configFile;
    }
    bool shouldExitAfterLastStep() const
    {
        return exitAfterLastStep;
    }

    /// @brief Print help message with available arguments.
    void printHelp() const;

private:
    std::vector<std::string> loadModelPaths;
    std::optional<std::string> startingModel;
    std::optional<std::string> generateMoviePath;
    std::optional<std::string> generateImagePath;
    std::optional<int> step;
    std::optional<std::string> configFile;
    bool exitAfterLastStep = false;
};
