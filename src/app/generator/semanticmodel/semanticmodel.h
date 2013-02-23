#ifndef SEMANTICMODEL_H
#define SEMANTICMODEL_H

#include <QList>
#include "robot.h"
//#include "module.h"

class SemanticModel
{
public:
    SemanticModel(Robot* r) { robot = r; }
    Robot* getRobot() const { return robot; }

    //QList<RobotModule*> getModules() { return modules; }
    //void addModule(RobotModule* mod) { if(mod && !modules.contains(mod)) modules << mod; }

private:
    Robot* robot;
    //Add in lists of module definitions, when those come in to development.
    //QList<RobotModule*> modules;
};

#endif // SEMANTICMODEL_H
