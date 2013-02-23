#include "bindgenerator.h"

BindGenerator::BindGenerator(Robot *robot, RobotBinding *binding) : robot(robot),binding(binding)
{
    static int count = 1;
    functionName = QString("callback%1").arg(count++);
    functionDeclaration = QString("inline void %1();").arg(functionName);
    functionDefinition = QString("void %1::%2()\n{%3}\n")
            .arg(robot->getClassName())
            .arg(functionName)
            .arg(getCodeForReceiver(binding->getReceiver(),binding->getForeignCode()));
}

QString BindGenerator::getCodeForReceiver(QString receiver, bool isForeignCode)
{
    if(!isForeignCode) {
        if(!receiver.startsWith("$"))
            return "\n";
        receiver = receiver.mid(1);
        QStringList path = receiver.split('/');
        if(path.size() != 2)
            qFatal("Invalid path to receiver!");
        RobotObject *target = 0;
        foreach(RobotObject* ro, robot->getObjects()) {
            if(ro->getName() == path.first())
                target = ro;
        }
        if(!target)
            qFatal(qPrintable(QString("No such object: %1").arg(path.first())));
        if(hasCapability(path.last())) {
            return "\n\t" + path.first() + "->" + path.last() + "();\n";
        }
    }
    return "\n\t" + receiver + "\n";
}

bool BindGenerator::hasCapability(const QString &name)
{
    return true; //for now.
}
