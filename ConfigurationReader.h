// configurationreader.h
#ifndef CONFIGURATIONREADER_H
#define CONFIGURATIONREADER_H

#include <QStringList>

class ConfigurationReader
{
public:
    static QStringList readNLinesFromFile(const char* fileName);
    // Aggiungi altri metodi di lettura e analisi dei dati, se necessario
};

#endif // CONFIGURATIONREADER_H
