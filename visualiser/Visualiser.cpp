#include <ranges>

#include "visualiser/Visualizer.hpp"


std::pair<int,int> VisualiserHelpers::getColumnAndRowFromLine(const std::string& line)
{
    /// input format: "C-R" where C and R are numbers
    if (line.empty())
    {
        throw std::invalid_argument("Line is empty, but it should dontain columns and row!");
    }

    const auto delimiterPos = line.find('-');
    if (delimiterPos == std::string::npos)
    {
        throw std::runtime_error("No delimiter '-' found in the line: >" + line + "<");
    }

    auto nLocalCols = std::stoi(line.substr(0, delimiterPos));
    auto nLocalRows = std::stoi(line.substr(delimiterPos + 1));

    return {nLocalCols, nLocalRows};
}

bool VisualiserHelpers::allNodesHaveEmptyData(const std::vector<int>& AlllocalCols, const std::vector<int>& AlllocalRows, int nodesCount)
{
    for (int node = 0; node < nodesCount; ++node)
    {
        if (AlllocalCols[node] > 0 || AlllocalRows[node] > 0)
        {
            return false;
        }
    }
    return true;
}

std::pair<int,int> VisualiserHelpers::calculateXYOffset(int node, int nNodeX, int nNodeY, const std::vector<int>& allLocalCols, const std::vector<int>& allLocalRows)
{
    int offsetX = 0; //= //(node % nNodeX)*nLocalCols;//-this->borderSizeX;
    int offsetY = 0; //= //(node / nNodeX)*nLocalRows;//-this->borderSizeY;

    if (nNodeY == 1)
    {
        for (int k = 0; k < node % nNodeX; k++)
        {
            offsetX += allLocalCols[k];
        }
    }
    else
    {
        for (int k = (node / nNodeX) * nNodeX; k < node; k++)
        {
            offsetX += allLocalCols[k];
        }
    }

    if (node >= nNodeX)
    {
        for (int k = node - nNodeX; k >= 0;)
        {
            offsetY += allLocalRows[k];
            k -= nNodeX;
        }
    }
    return {offsetX, offsetY};
}

std::vector<std::string_view> VisualiserHelpers::splitLine(std::string_view line, int nLocalCols, char delimiter) noexcept
{
    std::vector<std::string_view> result;
    result.reserve(nLocalCols);

    auto split_view = line | std::views::split(delimiter)
                      | std::views::filter([](auto&& rng) {
                            return !std::ranges::empty(rng);
                        });

    for (auto&& token : split_view)
    {
        result.emplace_back(token.begin(), token.end());
    }

    return result;
}

std::vector<int> VisualiserHelpers::splitLineIntoNumbers(const std::string& line, int nLocalCols, const char* separator)
{
    std::vector<int> numbers;
    numbers.reserve(nLocalCols);

    std::size_t currentPosition = {};
    while (true)
    {
        auto separatorPosition = line.find(separator, currentPosition);
        if (std::string::npos != separatorPosition)
        {
            auto currentElement2Convert = line.substr(currentPosition, separatorPosition);
            cout << ">" << currentElement2Convert << "<" << endl;
            auto currentNumber = std::stoi(currentElement2Convert);
            numbers.push_back(currentNumber);

            currentPosition = separatorPosition + 1;
        }
    }

    return numbers;
}
