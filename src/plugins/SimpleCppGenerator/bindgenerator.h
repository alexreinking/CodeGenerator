#ifndef BINDGENERATOR_H
#define BINDGENERATOR_H

#include <GeneratorPlugin.h>

class BindGenerator
{
public:
    BindGenerator(Robot* robot, RobotBinding* binding);

    QString getFunctionName() const { return functionName; }
    QString getFunctionDeclaration() const { return functionDeclaration; }
    QString getFunctionDefinition() const { return functionDefinition; }

    RobotBinding* getRobotBinding() const { return binding; }

private:
    QString functionName;
    QString functionDeclaration;
    QString functionDefinition;
    Robot* robot;
    RobotBinding* binding;

    QString getCodeForReceiver(QString receiver, bool isForeignCode);
    bool hasCapability(const QString& name);
};

#endif // BINDGENERATOR_H
