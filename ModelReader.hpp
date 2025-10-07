#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <climits> // INT_MAX
#include <cmath> // log10
#include <filesystem>
#include "types.h"
#include "visualiser/SettingParameter.h"
#include "visualiser/Line.h"


template <class Cell>
class ModelReader
{
    std::vector<std::unordered_map<StepIndex, FilePosition>> stage;

public:
    void prepareStage(NodeIndex nNodeX, NodeIndex nNodeY)
    {
        stage.resize(nNodeX * nNodeY);
    }

    void clearStage()
    {
        stage.clear();
    }

    template<class Matrix>
    void readStageStateFromFilesForStep(Matrix& m, SettingParameter* sp, Line *lines);

    /** @brief Loads data into hashMap from text files.
     *
     * Expected file format (each line):
     *     <stepNumber:int> <positionInFile:long>
     *
     * Example:
     *     0 37
     *     1 32504
     *     2 64971
     *
     * Each line must contain exactly two numbers separated by whitespace.
     *
     * @param nNodeX number of nodes along the X axis
     * @param nNodeY number of nodes along the Y axis
     * @param filename base filename (e.g., "ball"), for which nodes files are being read */
    void readStepsOffsetsForAllNodesFromFiles(NodeIndex nNodeX, NodeIndex nNodeY, const std::string &filename);

private:
    FilePosition getStepStartingPositionInFile(StepIndex step, NodeIndex node)
    {
        return stage.at(node).at(step);
    }

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
void ModelReader<T>::readStageStateFromFilesForStep(Matrix& m, SettingParameter* sp, Line *lines)
{
    const auto columnsAndRows = giveMeLocalColsAndRowsForAllSteps(sp->step, sp->nNodeX, sp->nNodeY, sp->outputFileName);

    bool startStepDone = false;

    constexpr std::size_t numbersPerLine = 10'000;
    const std::size_t lineBufferSize = (log10(UINT_MAX) + 2) * numbersPerLine;
    std::string line;
    line.reserve(lineBufferSize); /// for speedup to avoid realocations

    for (NodeIndex node = 0; node < sp->nNodeX * sp->nNodeY; ++node)
    {
        const auto offsetXY = ReaderHelpers::calculateXYOffset(node, sp->nNodeX, sp->nNodeY, columnsAndRows);

        ColumnAndRow columnAndRow;
        std::ifstream fp = readColumnAndRowForStepFromFileReturningStream(sp->step, sp->outputFileName, node, columnAndRow);
        if (! fp)
            throw std::runtime_error("Cannot open file for node " + std::to_string(node));

        /// introducing big buffer for reading (for speed up)
        static thread_local char fileBuffer[1 << 16];
        fp.rdbuf()->pubsetbuf(fileBuffer, sizeof(fileBuffer));

        lines[node * 2]     = Line(offsetXY.x(), offsetXY.y(), offsetXY.x() + columnAndRow.column, offsetXY.y());
        lines[node * 2 + 1] = Line(offsetXY.x(), offsetXY.y(), offsetXY.x(), offsetXY.y() + columnAndRow.row);

        for (int row = 0; row < columnAndRow.row; ++row)
        {
            if (! std::getline(fp, line))
            {
                const auto fileNameTmp = ReaderHelpers::giveMeFileName(sp->outputFileName, node);
                throw std::runtime_error("Error when reading entire line of file " + fileNameTmp);
            }

            std::replace(begin(line), end(line), ' ', '\0'); /// this is faster than using std::ranges::replace

            /// Moving through tokens (token are characters ended with '0')
            char* currentTokenPtr = line.data();
            for (int col = 0; col < columnAndRow.column && *currentTokenPtr; ++col)
            {
                if (! startStepDone) [[unlikely]]
                {
                    m[row + offsetXY.y()][col + offsetXY.x()].T::startStep(sp->step);
                    startStepDone = true;
                }

                char* nextTokenPtr = currentTokenPtr;
                while (*nextTokenPtr)
                    ++nextTokenPtr;
                ++nextTokenPtr; /// skip '\0'

                m[row + offsetXY.y()][col + offsetXY.x()].T::composeElement(currentTokenPtr);

                /// composeElement() may add extra '\0', so we need extra variable
                currentTokenPtr = nextTokenPtr;
            }
        }
    }
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

            const auto [it, inserted] = stage[node].emplace(stepNumber, positionInFile);
            if (! inserted)
            {
                std::cerr << std::format("Duplicate stepNumber {} found in file '{}' (node {})", stepNumber, fileNameIndex, node) << std::endl;
            }
        }
    }
}
