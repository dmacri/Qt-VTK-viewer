#include "visualiser/Visualizer.hpp"


std::pair<int,int> VisualiserHelpers::getColumnAndRowFromLine(const std::string& line)
{
    /// input format: "C-R" where C and R are numbers
    if (line.empty())
    {
        throw std::invalid_argument("Line is empty, but it should contain columns and row!");
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

std::pair<int,int> VisualiserHelpers::calculateXYOffset(NodeIndex node, int nNodeX, int nNodeY, const std::vector<int>& allLocalCols, const std::vector<int>& allLocalRows)
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
