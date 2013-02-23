#ifndef FRCCODEGENERATOR_H
#define FRCCODEGENERATOR_H

#include <QtCore>
#include "interfaces/generatorinterface.h"

class FRCCodeGenerator
{
public:
    FRCCodeGenerator(const QString &targetLanguage, QDir pluginsDir = QDir());
    ~FRCCodeGenerator() {}
    bool generate(const QDir& projectDir);
    bool setInputFile(const QString& fileName);
    void setLanguage(const QString &lang);

    QStringList getLanguages() const { return languages; }

private:
    GeneratorInterface* selectedGenerator;
    QString input;
    QStringList languages;
    QDir pluginsDir;
};

#endif // FRCCODEGENERATOR_H
