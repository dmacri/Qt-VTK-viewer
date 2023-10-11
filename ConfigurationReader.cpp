// configurationreader.cpp
#include "ConfigurationReader.h"
#include <QFile>
#include <QTextStream>

QStringList ConfigurationReader::readNLinesFromFile(const char* fileName)
{
    QStringList dataLines;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            dataLines.append(line);
        }
        file.close();
    }

    return dataLines;
}
