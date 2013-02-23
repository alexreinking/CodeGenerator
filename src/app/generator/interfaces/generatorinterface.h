#ifndef GENERATORINTERFACE_H
#define GENERATORINTERFACE_H

#include <QDir>
#include "../datastructures/ast.h"
class SemanticModel;

class GeneratorInterface
{
public:
    virtual ~GeneratorInterface() {}
    virtual void setProjectDirectory(const QDir &dir) = 0;
    virtual QStringList getLanguage() const = 0;

    virtual bool generate(SemanticModel* ast) = 0;
};

Q_DECLARE_INTERFACE(GeneratorInterface, "com.reinking.frc.GeneratorInterface/1.0")

#endif // GENERATORINTERFACE_H
