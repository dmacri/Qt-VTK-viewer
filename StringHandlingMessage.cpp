#include "QSettings"
#include <QMap>
#include <QString>
#include <QDebug>

class StringHandlingMessage {
public:
    static QMap<QString, QString> loadStrings(QString iniFilePath) {
        QMap<QString, QString> stringMap;

        //  QString iniFilePath = QApplication::applicationDirPath() + "/app_strings.ini";
        QSettings settings(iniFilePath, QSettings::IniFormat);
        qDebug() << "Il file path è" << settings.fileName();

        stringMap["noSelectionMessage"] = settings.value("Messages/noSelectionWarning").toString();
        stringMap["directorySelectionMessage"] = settings.value("Messages/directorySelectionWarning").toString();
        stringMap["compilationSuccessfulMessage"] = settings.value("Messages/compilationSuccessful").toString();
        stringMap["compilationFailedMessage"] = settings.value("Messages/compilationFailed").toString();
        stringMap["deleteSuccessfulMessage"] = settings.value("Messages/deleteSuccessful").toString();
        stringMap["deleteFailedMessage"] = settings.value("Messages/deleteFailed").toString();

        return stringMap;
    }
};
