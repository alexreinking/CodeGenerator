#include "eventdispatchergenerator.h"

EventDispatcherGenerator::EventDispatcherGenerator(Robot *r, const QDir &dir)
{
    m_robot = r;
    outDir = dir;
    teleopClassName = "";
}

bool EventDispatcherGenerator::generate()
{
    bool ok = generateEDHeader() && generateEDSource();
#ifdef Q_OS_LINUX
    qDebug("Generated event dispatcher.");
#endif
    return ok;
}

void EventDispatcherGenerator::addJoystick(Joystick *joy)
{
    joys.append(joy);
}

bool EventDispatcherGenerator::generateEDHeader()
{
    QString out =
            "#ifndef EVENTDISPATCHER_H\n"
            "#define EVENTDISPATCHER_H\n\n"
            "#include \"WPILib.h\"\n"
            "#include <vector>\n"
            "using namespace std;\n\n"
            "class Event;\n"
            "class EventListener;\n"
            "class RobotMode;\n"
            "class " + teleopClassName + ";\n"
            "class DisabledRobot;\n\n"
            "class EventDispatcher : public IterativeRobot\n{\n"
            "public:\n\t"
            "EventDispatcher();\n\n\t"
            "void RobotInit() {};\n\n\t"
            "void DisabledInit();\n\t"
            "void DisabledPeriodic() {}\n\t"
            "void DisabledContinuous() {}\n\n\t"
            "void AutonomousInit();\n\t"
            "void AutonomousPeriodic() {}\n\t"
            "void AutonomousContinuous() {}\n\n\t"
            "void TeleopInit();\n\t"
            "void TeleopPeriodic() {}\n\t"
            "void TeleopContinuous();\n\n\t"
            "void deleteAllListeners();\n\t"
            "void sendEvent(Event *e);\n\n"
            "private:\n\t"
            "RobotMode *robot;\n\t" +
            teleopClassName + " *teleoperatedRobot;\n\t"
            "DisabledRobot *disabledRobot;\n\t"
            "vector<EventListener*> listeners;\n\t"
            "vector<Event*> events;\n};\n\n"
            "#endif\n";
    QFile f(outDir.absoluteFilePath("EventDispatcher.h"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << out;
    } else {
        qFatal("Could not create EventDispatcher.h! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}

bool EventDispatcherGenerator::generateEDSource()
{
    QString out =
            "#include \"EventDispatcher.h\"\n"
            "#include \"drive.h\"\n"
            "#include \"teleoperatedrobot.h\"\n"
            "#include \"JoystickListener.h\"\n"
            "#include \"RobotError.h\"\n\n"
            "EventDispatcher::EventDispatcher()\n{\n\t"
            "disabledRobot = new DisabledRobot();\n\t"
            "teleoperatedRobot = new " + teleopClassName + "();\n\t"
            "robot = disabledRobot;\n\n\t";
    foreach(Joystick* j, joys) {
        out += "listeners.push_back(new JoystickListener(this,"
                + ((j->getJoystickType() == Joystick::Extreme3D) ? QString("Extreme3DPro") : QString("Attack3"))
                + "," + QString("%1").arg(j->getPort()) + ");\n";
    }
    out  += "}\n\nvoid EventDispatcher::DisabledInit()\n{\n\t"
            "robot = disabledRobot;\n\t"
            "teleoperatedRobot->disable();\n}\n\n"
            "void EventDispatcher::AutonomousInit()\n{\n\t"
            "robot = disabledRobot;\n}\n\n"
            "void EventDispatcher::TeleopInit()\n{\n\t"
            "robot = teleoperatedRobot;\n}\n\n"
            "void EventDispatcher::TeleopContinuous()\n{\n\t"
            "static bool active = true;\n\t"
            "if(active) {\n\t\t"
            "for(unsigned int i = 0; i < listeners.size(); i++) {\n\t\t\t"
            "if(listeners[i]) listeners[i]->update(); //Refactor to poll()\n\t\t}\n\t\t"
            "for(unsigned int i = 0; i < events.size(); i++) { \n\t\t\t"
            "if(!robot->handle(events[i])) {\n\t\t\t\t"
            "RobotError* err = robot->lastError();\n\t\t\t\t"
            "if(err->getErrorLevel() == Fatal) active = false;\n\t\t\t}\n\t\t\t"
            "delete events[i];\n\t\t\tevents[i] = 0;\n\t\t}\n\t\t"
            "events.clear();\n\t}\n}\n\n"
            "void EventDispatcher::deleteAllListeners()\n{\n\t"
            "for(unsigned int i = 0; i < listeners.size(); i++)\n\t\t"
            "delete listeners[i];\n\t"
            "listeners.clear();\n}\n\n"
            "void EventDispatcher::sendEvent(Event *e)\n{\n\t"
            "if(e) events.push_back(e);\n}\n\n"
            "START_ROBOT_CLASS(EventDispatcher);\n";
    QFile f(outDir.absoluteFilePath("EventDispatcher.cpp"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << out;
    } else {
        qFatal("Could not create EventDispatcher.cpp! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}
