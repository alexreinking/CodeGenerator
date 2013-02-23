#include "teleopgenerator.h"

TeleopGenerator::TeleopGenerator(Robot *r, const QDir &dir)
{
    robot = r;
    projectDir = dir;
    className = r->getClassName() + "_Teleoperated";
    driveClassName = "";
}

void TeleopGenerator::setDriveClassName(const QString &dcn)
{
    driveClassName = dcn;
}

bool TeleopGenerator::generate()
{
    bool ok = generateTeleopHeader() && generateTeleopSource();
#ifdef Q_OS_LINUX
    qDebug("Generated mode class \'%s\'.",qPrintable(className));
#endif
    return ok;
}

bool TeleopGenerator::generateTeleopHeader()
{
    QString ret =
            "#ifndef TELEOPERATED_ROBOT\n"
            "#define TELEOPERATED_ROBOT\n\n"
            "#include \"RobotMode.h\"\n"
            "#include \"Event.h\"\n"
            "#include \"drive.h\"\n\n"
            "class " + className + " : public RobotMode\n{\n"
            "public:\n\t" +
            className + "();\n\t"
            "~" + className + "();\n\n\t"
            "bool handle(Event *e);\n\t"
            "RobotError* lastError() { return myError; }\n\n\t"
            "void disable();\n\n"
            "private:\n\t" +
            driveClassName + " *drive;\n\t"
            "RobotError *myError;\n"
            "};\n";
    QFile f(projectDir.absoluteFilePath("teleoperatedrobot.h"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << ret.replace('\t',"    ");
    } else {
        qFatal("Could not create teleoperatedrobot.h! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}

bool TeleopGenerator::generateTeleopSource()
{
    QString ret =
            "#include \"teleoperatedrobot.h\"\n\n" +
            className + "::" + className + "()\n{\n\t"
            "drive = new " + driveClassName + ";\n}\n\n" +
            className + "::~" + className + "()\n{\n\t"
            "delete drive;\n\t"
            "delete myError;\n}\n\n"
            "bool " + className + "::handle(Event *e)\n{\n\t"
            "JoystickPositionEvent *jpe = 0;\n\n\t"
            "if(!e) {\n\t\t"
            "if(myError) delete myError;\n\t\t"
            "myError = new RobotError(Warning, \"" + className + " received a null ptr!\");\n\t\t"
            "return false;\n\t}\n\n\t"
            "switch((int)e->type()) {\n\t"
            "case JoystickPosition:\n\t\t"
            "jpe = static_cast<JoystickPositionEvent*>(e);\n\t\t"
            "drive->Drive(jpe->x(),jpe->y(),jpe->twist());\n\t\t"
            "break;\n\t"
            "default:\n\t\t"
            "if(myError) delete myError;\n\t\t"
            "myError = new RobotError(Warning, \"" + className + " received unknown event type.\");\n\t\t"
            "return false;\n\t\t"
            "break;\n\t}\n\treturn true;\n}\n";
    QFile f(projectDir.absoluteFilePath("teleoperatedrobot.cpp"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << ret.replace('\t',"    ");
    } else {
        qFatal("Could not create teleoperatedrobot.h! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}
