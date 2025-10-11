/** @file ModelReader.hpp
 * @brief Declaration of the ModelReader template class for reading and processing model data.
 * 
 * This file contains the ModelReader class template which provides functionality to read,
 * parse, and process model data from files. It supports reading data in stages and provides
 * methods to access model data at different time steps. */

#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <format>
#include <climits> // INT_MAX
#include <cmath>   // log10
#include <filesystem>
#include <algorithm> // std::ranges::sort
#include <future>
#include <ranges>
#include "types.h"
#include "visualiser/SettingParameter.h"
#include "visualiser/Line.h"

/** @class ModelReader
 * @brief Template class for reading and processing model data from files.
 * 
 * The ModelReader class provides functionality to read model data in stages and
 * access it at different time steps. It's designed to work with different cell types
 * through template specialization.
 * 
 * @tparam Cell The cell type used in the model 
 * Note: Cell is derived class from Element (which is header from OOpenCal) */
template <class Cell>
class ModelReader
{
    std::vector<std::unordered_map<StepIndex, FilePosition>> nodeStepOffsets; ///< Maps node indices to their file positions for each step

public:
    /** @brief Prepares the reader for a new stage of data processing.
     * 
     * Initializes the internal data structures to handle a grid of nodes with
     * the specified dimensions.
     * 
     * @param nNodeX Number of nodes along the X axis
     * @param nNodeY Number of nodes along the Y axis */
    void prepareStage(NodeIndex nNodeX, NodeIndex nNodeY)
    {
        nodeStepOffsets.resize(nNodeX * nNodeY);
    }

    /// @brief Clears the current stage and releases associated resources.
    void clearStage()
    {
        nodeStepOffsets.clear();
    }

    /** @brief Reads the stage state from files for a specific step.
     * 
     * This method reads the model state for a specific simulation step and updates
     * the provided matrix and settings accordingly.
     * 
     * @tparam Matrix The matrix type used to store the model state
     * @param m Reference to the matrix that will store the model state
     * @param sp Pointer to the setting parameters
     * @param lines Pointer to the line data structure */
    template<class Matrix>
    void readStageStateFromFilesForStep(Matrix& m, SettingParameter* sp, Line* lines);

    /** @brief Loads step offset data from text files into an internal hash map.
     *
     * This method reads a file containing step number to file position mappings
     * for each node in the simulation. The file format is expected to have one
     * line per step per node with the format:
     *     <stepNumber:int> <positionInFile:long>
     *
     * Example:
     *     0 37
     *     1 32504
     *     2 64971
     *
     * Each line must contain exactly two numbers separated by whitespace.
     *
     * @param nNodeX Number of nodes along the X axis
     * @param nNodeY Number of nodes along the Y axis
     * @param filename Name of the file containing the step offsets
     * 
     * @throws std::runtime_error If the file cannot be opened or has an invalid format */
    void readStepsOffsetsForAllNodesFromFiles(NodeIndex nNodeX, NodeIndex nNodeY, const std::string& filename);

    /** @brief Returns a sorted list of all available simulation steps.
     *
     * This method inspects the internal `stage` structure, which stores for each node
     * a mapping from step number (`StepIndex`) to byte offset within its corresponding file.
     * It verifies that all nodes contain the same set of step indices. If any mismatch
     * between nodes is detected, it either throws an exception or prints a warning message
     * to `std::cerr`, depending on the value of `throwOnMismatch`.
     *
     * Additionally, if the `stage` data structure is empty (no node data loaded),
     * the function will either throw a `std::runtime_error` or print a warning,
     * depending on the value of `throwOnMismatch`.
     *
     * @param throwOnMismatch If true, throws `std::runtime_error` when:
     *        - a mismatch between node step sets is detected, or
     *        - the `stage` structure is empty.
     *        If false, prints warnings to `std::cerr` instead of throwing.
     *
     * @return A sorted `std::vector<StepIndex>` containing all available simulation steps.
     *
     * @throws std::runtime_error If `throwOnMismatch` is true and:
     *         - the `stage` is empty, or
     *         - the step sets differ between nodes. */
    std::vector<StepIndex> availableSteps(bool throwOnMismatch=false) const;

private:
    FilePosition getStepStartingPositionInFile(StepIndex step, NodeIndex node) const;

    /** @brief Opens the data file for a given simulation step and node.
     *
     * The function locates the correct file for the specified node (e.g. "ball3.txt", where 3 is node number),
     * seeks to the byte position corresponding to the given simulation step in file,
     * reads the first header line containing local grid dimensions (columns and rows),
     * and returns an input file stream positioned right after that header.
     *
     * @param step         Simulation step number.
     * @param fileName     Base file name (without node index or extension).
     * @param node         Node index for which data should be opened.
     * @param columnAndRow Output: number of local columns and rows read from header line.
     *
     * @return std::ifstream Stream ready for reading cell data of this node at given step.
     * @throws std::runtime_error If the file cannot be opened, seek fails, or header is invalid. */
    [[nodiscard]] std::ifstream readColumnAndRowForStepFromFileReturningStream(StepIndex step, const std::string& fileName, NodeIndex node, ColumnAndRow& columnAndRow);

    [[nodiscard]] ColumnAndRow readColumnAndRowForStepFromFile(StepIndex step, const std::string& fileName, NodeIndex node);

    std::vector<ColumnAndRow> giveMeLocalColsAndRowsForAllSteps(StepIndex step, int nNodeX, int nNodeY, const std::string& fileName);
};

/////////////////////////////
namespace ReaderHelpers /// functions which are not templates
{
[[nodiscard]] inline std::string giveMeFileName(const std::string& fileName, NodeIndex node)
{
    return std::format("{}{}.txt", fileName, node);
}
[[nodiscard]] inline std::string giveMeFileNameIndex(const std::string &fileName, NodeIndex node)
{
    return std::format("{}{}_index.txt", fileName, node);
}

ColumnAndRow getColumnAndRowFromLine(const std::string& line);

ColumnAndRow calculateXYOffset(NodeIndex node, int nNodeX, int nNodeY, const std::vector<ColumnAndRow> &columnsAndRows);
}
/////////////////////////////

template <class T>
ColumnAndRow ModelReader<T>::readColumnAndRowForStepFromFile(StepIndex step, const std::string& fileName, NodeIndex node)
{
    ColumnAndRow columnAndRow;
    std::ifstream file = readColumnAndRowForStepFromFileReturningStream(step, fileName, node, columnAndRow);
    return columnAndRow;
}
template <class T>
std::ifstream ModelReader<T>::readColumnAndRowForStepFromFileReturningStream(StepIndex step, const std::string& fileName, NodeIndex node, ColumnAndRow &columnAndRow)
{
    const auto fileNameTmp = ReaderHelpers::giveMeFileName(fileName, node);

    std::ifstream file(fileNameTmp);
    if (! file.is_open())
    {
        throw std::runtime_error(std::format("Can't read '{}' in {} function", fileNameTmp, __func__));
    }

    const auto fPos = getStepStartingPositionInFile(step, node);
    file.seekg(fPos);
    if (! file)
    {
        throw std::runtime_error(std::format("Seek failed in '{}' at position {}", fileNameTmp, fPos));
    }

    std::string line;
    if (! std::getline(file, line))
    {
        throw std::runtime_error(std::format("Failed to read line from '{}' at position {}", fileNameTmp, fPos));
    }

    columnAndRow = ReaderHelpers::getColumnAndRowFromLine(line);
    return file;
}

template<class T>
template<class Matrix>
void ModelReader<T>::readStageStateFromFilesForStep(Matrix& m, SettingParameter* sp, Line* lines)
{
    const auto totalNodes = sp->nNodeX * sp->nNodeY;
    const auto columnsAndRows = giveMeLocalColsAndRowsForAllSteps(sp->step, sp->nNodeX, sp->nNodeY, sp->outputFileName);

    /// Lambda responsible for reading and processing a single node’s file
    auto processNode = [&, this/*, m, sp, lines, columnsAndRows*/](NodeIndex node) /*mutable*/
    {
        const auto offsetXY = ReaderHelpers::calculateXYOffset(node, sp->nNodeX, sp->nNodeY, columnsAndRows);

        ColumnAndRow columnAndRow;
        std::ifstream fp = readColumnAndRowForStepFromFileReturningStream(sp->step, sp->outputFileName, node, columnAndRow);
        if (! fp)
            throw std::runtime_error("Cannot open file for node " + std::to_string(node));

        // Use a thread-local buffer for faster reading
        static thread_local char fileBuffer[1 << 16];
        fp.rdbuf()->pubsetbuf(fileBuffer, sizeof(fileBuffer));

        // Define boundary lines for the node
        lines[node * 2]     = Line(offsetXY.x(), offsetXY.y(), offsetXY.x() + columnAndRow.column, offsetXY.y());
        lines[node * 2 + 1] = Line(offsetXY.x(), offsetXY.y(), offsetXY.x(), offsetXY.y() + columnAndRow.row);

        // Reserve a large line buffer to minimize reallocations
        constexpr std::size_t numbersPerLine = 10'000;
        const std::size_t lineBufferSize = (std::log10(UINT_MAX) + 2) * numbersPerLine;
        std::string line;
        line.reserve(lineBufferSize);

        bool localStartStepDone = false;

        // Process each line (row) from the node’s file
        for (int row = 0; row < columnAndRow.row; ++row)
        {
            if (! std::getline(fp, line))
            {
                const auto fileNameTmp = ReaderHelpers::giveMeFileName(sp->outputFileName, node);
                throw std::runtime_error("Error reading entire line from " + fileNameTmp);
            }

            // Replace spaces with '\0' to tokenize more efficiently
            std::replace(line.begin(), line.end(), ' ', '\0');  /// this is faster than using std::ranges::replace

            // Tokenize and fill the corresponding part of the matrix
            char* currentTokenPtr = line.data();
            for (int col = 0; col < columnAndRow.column && *currentTokenPtr; ++col)
            {
                if (!localStartStepDone) [[unlikely]]
                {
                    m[row + offsetXY.y()][col + offsetXY.x()].T::startStep(sp->step);
                    localStartStepDone = true;
                }

                /// composeElement() may add extra '\0', so we need extra variable to jump to next position
                char* nextTokenPtr = std::find(currentTokenPtr, line.data() + line.size(), '\0');
                ++nextTokenPtr; // skip '\0'

                m[row + offsetXY.y()][col + offsetXY.x()].T::composeElement(currentTokenPtr);

                currentTokenPtr = nextTokenPtr;
            }
        }
    };

    /// Launch async tasks — one per node
    std::vector<std::future<void>> futures;
    futures.reserve(totalNodes);

    for (NodeIndex node = 0; node < totalNodes; ++node)
    {
        futures.push_back(std::async(std::launch::async, [&, node]() {
            processNode(node);
        }));
    }

    // Wait for all async tasks to complete
    std::ranges::for_each(futures, [](std::future<void>& f){ f.get(); });
}
template<class T>
std::vector<ColumnAndRow> ModelReader<T>::giveMeLocalColsAndRowsForAllSteps(StepIndex step, int nNodeX, int nNodeY, const std::string& fileName)
{
    const auto nodesCount = nNodeX * nNodeY;
    std::vector<ColumnAndRow> allColumnsAndRows(nodesCount);
    allColumnsAndRows.resize(nodesCount);

    for (NodeIndex node = 0; node < nodesCount; node++)
    {
        allColumnsAndRows[node] = readColumnAndRowForStepFromFile(step, fileName, node);
    }
    return allColumnsAndRows;
}

template <class T>
void ModelReader<T>::readStepsOffsetsForAllNodesFromFiles(NodeIndex nNodeX, NodeIndex nNodeY, const std::string& filename)
{
    const auto totalNodes = nNodeX * nNodeY;
    for (NodeIndex node = 0; node < totalNodes; ++node)
    {
        const auto fileNameIndex = ReaderHelpers::giveMeFileNameIndex(filename, node);
        std::cout << "Reading file: " << fileNameIndex << '\n';

        if (! std::filesystem::exists(fileNameIndex))
        {
            throw std::runtime_error("File not found: " + fileNameIndex);
        }

        std::ifstream file(fileNameIndex);
        if (! file)
        {
            throw std::runtime_error("Cannot open file: " + fileNameIndex);
        }

        while (true)
        {
            StepIndex stepNumber;
            FilePosition positionInFile;

            if (! (file >> stepNumber >> positionInFile))
            { /// enters here if reading error
                if (file.eof())
                    break;
                else
                    throw std::runtime_error("Invalid line format in file: " + fileNameIndex);
            }

            const auto [it, inserted] = nodeStepOffsets[node].emplace(stepNumber, positionInFile);
            if (! inserted)
            {
                std::cerr << std::format("Duplicate stepNumber {} found in file '{}' (node {})", stepNumber, fileNameIndex, node) << std::endl;
            }
        }
    }
}

template<class Cell>
std::vector<StepIndex> ModelReader<Cell>::availableSteps(bool throwOnMismatch) const
{
    if (nodeStepOffsets.empty())
    {
        const auto errorMessage = "Warning: availableSteps() called on an empty stage.";
        if (throwOnMismatch)
        {
            throw std::runtime_error(errorMessage);
        }
        else
        {
            std::cerr << errorMessage << std::endl;
        }
        return {};
    }

    // Helper lambda: extracts all step indices (keys) from a map and returns them sorted.
    auto extractAndSortStepIndices = [](const auto& map) -> std::vector<StepIndex>
    {
        auto steps = std::views::keys(map);
        std::vector<StepIndex> steps2Return(steps.begin(), steps.end());
        std::ranges::sort(steps2Return);
        return steps2Return;
    };

    // Use the first node as the reference
    const auto& fistNodeData = nodeStepOffsets.front();

    // Collect and sort all step indices from the first node
    auto firstNodeSteps = extractAndSortStepIndices(fistNodeData);

    // Compare each node's step list against the reference
    for (NodeIndex node = 1; node < nodeStepOffsets.size(); ++node)
    {
        const auto& nodeMap = nodeStepOffsets[node];
        if (nodeMap.size() != fistNodeData.size())
        {
            const std::string msg = std::format("Step count mismatch for node {} (expected {}, found {})",
                                                node, fistNodeData.size(), nodeMap.size());
            if (throwOnMismatch)
                throw std::runtime_error(msg);
            else
                std::cerr << "Warning: " << msg << '\n';
        }

        // Extract steps for comparison
        auto nodeSteps = extractAndSortStepIndices(nodeMap);

        // Compare with reference set
        if (! std::ranges::equal(firstNodeSteps, nodeSteps))
        {
            const std::string msg = std::format("Inconsistent step indices detected in node {}.", node);
            if (throwOnMismatch)
                throw std::runtime_error(msg);
            else
                std::cerr << "[Warning] " << msg << '\n';
        }
    }

    // Return the sorted list of unique steps
    return firstNodeSteps;
}

template<class Cell>
FilePosition ModelReader<Cell>::getStepStartingPositionInFile(StepIndex step, NodeIndex node) const
{
    if (node >= nodeStepOffsets.size())
    {
        throw std::out_of_range(std::format("Invalid node index {} (available nodes: {})", node, nodeStepOffsets.size()));
    }

    const auto& stepMap = nodeStepOffsets[node];
    if (auto it = stepMap.find(step); it != stepMap.end())
    {
        return it->second;
    }

    throw std::out_of_range(std::format("Step {} not found in node {} (available step indices: {})", step, node, stepMap.size() - 1));
}
