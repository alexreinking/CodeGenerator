#include "cppgenerator.h"

void CppGenerator::setProjectDirectory(const QDir &dir)
{
    projectDir = dir;
    projectDir.cd("src");
}

bool CppGenerator::generate(SemanticModel *sm)
{
    dg = new DriveGenerator(sm->getRobot()->getDriveHandler(),projectDir);
    tg = new TeleopGenerator(sm->getRobot(),projectDir);
    tg->setDriveClassName(dg->getClassName());
    edg = new EventDispatcherGenerator(sm->getRobot(),projectDir);
    edg->setTeleopClassName(tg->getClassName());

    QList<RobotObject*> objs = sm->getRobot()->getObjects();
    foreach(RobotObject *obj, objs) {
        if(!obj)
            continue;
        else if(obj->getType() == RobotObject::JoystickObject)
            edg->addJoystick(static_cast<Joystick*>(obj));
    }

    bool ret = (dg->generate() && tg->generate() && edg->generate());
    delete dg;
    delete tg;
    delete edg;
    return ret;
}

QString CppGenerator::deleteObj(RobotObject *ro)
{
    return "delete " + ro->getName() + ";";
}

Q_EXPORT_PLUGIN2(cppgen, CppGenerator)
