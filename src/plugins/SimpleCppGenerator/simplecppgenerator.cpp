#include "simplecppgenerator.h"

void SimpleCPPGenerator::setProjectDirectory(const QDir &dir)
{
    projectDir = dir;
    if(!projectDir.exists("src")) {
        projectDir.mkdir("src");
    }
    projectDir.cd("src");
}

bool SimpleCPPGenerator::generate(SemanticModel *sm)
{
    QList<RobotObject*> objs = sm->getRobot()->getObjects();
    QList<Wheel*> wheels;
    QList<Wheel*> otherWheels;
    Wheel* FL = sm->getRobot()->getDriveHandler()->getWheel(DriveHandler::FrontLeft);
    Wheel* FR = sm->getRobot()->getDriveHandler()->getWheel(DriveHandler::FrontRight);
    Wheel* BL = sm->getRobot()->getDriveHandler()->getWheel(DriveHandler::BackLeft);
    Wheel* BR = sm->getRobot()->getDriveHandler()->getWheel(DriveHandler::BackRight);
    QList<RobotIndicator*> indicators;
    wheels << FL << FR << BL << BR;

    QString headerContent =
            "#ifndef ROBOT_MAIN\n"
            "#define ROBOT_MAIN\n\n"
            "#include \"WPILib.h\"\n"
            "#include \"JoystickWrapper.h\"\n"
            "#include \"JoystickCallback.h\"\n"
            "#include \"DisplayWrapper.h\"\n"
            "#include <math.h>\n\n"
            "class " + sm->getRobot()->getClassName() + " : public SimpleRobot\n{\n"
            "public:\n\t" + sm->getRobot()->getClassName() + "();\n\t"
            "~" + sm->getRobot()->getClassName() + "();\n\t"
            "void Autonomous() {}\n\t"
            "void OperatorControl();\n\n"
            "private:\n\t";
    foreach(RobotObject *obj, objs) {
        if(!obj)
            continue;
        else if(obj->getType() == RobotObject::JoystickObject) {
            headerContent += "JoystickWrapper *" + obj->getName() + ";\n\t";
            headerContent += "JoystickCallback<" + sm->getRobot()->getClassName() + "> *" + obj->getName() + "Callback;\n\t";
        } else if(obj->getType() == RobotObject::IndicatorObject) {
            indicators.append(static_cast<RobotIndicator*>(obj));
        } else if(obj->getType() == RobotObject::GyroObject) {
            headerContent += "Gyro* " + obj->getName() + ";\n\t";
        } else if(obj->getType() == RobotObject::BindingObject) {
            RobotBinding* rb = static_cast<RobotBinding*>(obj);
            if(!rb) continue;
            BindGenerator bg(sm->getRobot(),rb);
            bindings << bg;
            headerContent += bg.getFunctionDeclaration() += "\n\t";
            qWarning(qPrintable(QString("Robot bindings under development! Sender: %1 Receiver: %2 Foreign Code: %3").arg(rb->getSender()).arg(rb->getReceiver()).arg(rb->getForeignCode())));
        } else if(obj->getType() == RobotObject::WheelObject) {
            Wheel* wh = static_cast<Wheel*>(obj);
            if(!wheels.contains(wh)) {
                headerContent += "\n\t" + wh->getMotorType().mid(1) + "* " + wh->getName() + ";";
                otherWheels << wh;
            }
        }
    }

    QList<Wheel*> allWheels = wheels;
    allWheels.append(otherWheels);
    foreach(Wheel* wh, allWheels) {
        if(wh)
            headerContent += "\n\t" + wh->getMotorType().mid(1) + "* " + wh->getName() + ";";
    }

    headerContent += "\n\n\tvoid Drive(";
    if(sm->getRobot()->getDriveHandler()->getMode() == "Tank")
        headerContent += "float __left, float __right";
    else if(sm->getRobot()->getDriveHandler()->getMode() == "Mecanum")
        headerContent += "float __x, float __y, float __rotation";
    else //Default to arcade.
        headerContent += "float __x, float __y";
    headerContent += ");\n";

    //Additional Mecanum functions:
    if(sm->getRobot()->getDriveHandler()->getMode() == "Mecanum") {
        headerContent += "\tvoid Normalize(double *wheelSpeeds);\n\t";
        headerContent += "void RotateVector(double &x, double &y, double angle);";
    }

    headerContent += "\n};\n\n#endif\n";

    QFile headerFile(projectDir.absoluteFilePath("robot.h"));
    if(headerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("Generating SimpleRobot header file...");
        QTextStream of(&headerFile);
        of << headerContent.replace('\t',"    ");
    } else {
        qFatal("Could not create robot.h! Reason: %s",qPrintable(headerFile.errorString()));
    }
    headerFile.close();

    QString sourceContent =
            "#include \"robot.h\"\n\n#define GET_FUNC(x) &" + sm->getRobot()->getClassName() + "::x\n\n" +
            sm->getRobot()->getClassName() + "::" + sm->getRobot()->getClassName() + "()\n{\n\t";

    foreach(Wheel* wh, allWheels) {
        if(wh)
            sourceContent += constructWheel(wh) + "\n\t";
    }

    foreach(RobotObject *obj, objs) {
        if(!obj)
            continue;
        else if(obj->getType() == RobotObject::JoystickObject) {
            sourceContent += "\n\t" + obj->getName() + " = new JoystickWrapper("
                    + QString("%1").arg(static_cast<Joystick*>(obj)->getPort())
                    + ", " + ((static_cast<Joystick*>(obj)->getJoystickType() == Joystick::Extreme3D) ? QString("Extreme3DPro") : QString("Attack3")) + ");";
            sourceContent += "\n\t" + obj->getName() + "Callback = new JoystickCallback<" + sm->getRobot()->getClassName() + ">(" + obj->getName() + ",this);";
        }
        else if(obj->getType() == RobotObject::GyroObject) {
            sourceContent += QString("\n\t%1 = new Gyro(%2);\n\t%1->Reset();\n").arg(obj->getName()).arg(static_cast<Gyro*>(obj)->getChannel());
        }
    }

    foreach(BindGenerator bg, bindings)
    {
        sourceContent += "\n\t" + getJoystickBindingStatement(bg.getRobotBinding()->getSender()).arg(bg.getFunctionName());
    }

    sourceContent += "\n}\n\n" + sm->getRobot()->getClassName() + "::~" + sm->getRobot()->getClassName() + "()\n{";
    foreach(RobotObject *obj, objs) {
        if(obj && obj->getType() != RobotObject::IndicatorObject && !obj->getName().isEmpty())
            sourceContent += "\n\t" + deleteObj(obj);
    }
    sourceContent += "\n}\n\n";

    sourceContent += "void " + sm->getRobot()->getClassName() + "::OperatorControl()\n{\n\twhile(IsOperatorControl()) {\n\t\tfloat x,y;\n\t\t";
    sourceContent += sm->getRobot()->getDriveHandler()->getJoystick(DriveHandler::PrimaryJoystick)->getName() + "->GetAxis(&x,&y);\n\t\t";

    if(sm->getRobot()->getDriveHandler()->getMode() == "Tank") {
        sourceContent += "float x2,y2;\n\t\t";
        sourceContent += sm->getRobot()->getDriveHandler()->getJoystick(DriveHandler::SecondaryJoystick)->getName() + "->GetAxis(&x2,&y2);\n\t\t";
    }

    sourceContent += "\n\t\tDrive(";
    if(sm->getRobot()->getDriveHandler()->getMode() == "Tank") {
        sourceContent += "y,y2";
    } else {
        sourceContent += "x,y" + ((sm->getRobot()->getDriveHandler()->getMode() == "Mecanum") ?
                                      "," + sm->getRobot()->getDriveHandler()->getJoystick(DriveHandler::PrimaryJoystick)->getName() + "->GetRotation()" :
                                      "");
    }
    sourceContent += ");\n\n\t\t";

    foreach(RobotIndicator* ri, indicators) {
        sourceContent += QString("DisplayWrapper::GetInstance()->PrintfLine(%1,\"%3%4\", %5);\n\t\t").arg(ri->getLine()).arg(ri->getName()).arg(ri->getFormatString()).arg(ri->getCode());
    }
    if(indicators.size()) {
        sourceContent += "DisplayWrapper::GetInstance()->Output();\n\t\t";
    }
    QStringList processedSenders;
    foreach(BindGenerator bg, bindings) {
        QString sender = bg.getRobotBinding()->getSender().mid(1).split('/').first();
        if(!processedSenders.contains(sender)) {
            sourceContent += sender + "Callback->Update();\n\t\t";
            processedSenders << sender;
        }
    }

    sourceContent += "\n\t\tWait(0.01);\n\t}\n}\n\n";

    sourceContent += "void " + sm->getRobot()->getClassName() + "::Drive(";

    if(sm->getRobot()->getDriveHandler()->getMode() == "Tank") {
        sourceContent += "float __left, float __right)\n{\n";
        if(FL) sourceContent += "\t" + FL->getName() + "->Set(" + ((FL->getInverted() == "%t") ? "-1*" : "") + "__left);\n";
        if(BL) sourceContent += "\t" + BL->getName() + "->Set(" + ((BL->getInverted() == "%t") ? "-1*" : "") + "__left);\n";
        if(FR) sourceContent += "\t" + FR->getName() + "->Set(" + ((FR->getInverted() == "%t") ? "-1*" : "") + "__right);\n";
        if(BR) sourceContent += "\t" + BR->getName() + "->Set(" + ((BR->getInverted() == "%t") ? "-1*" : "") + "__right);\n";
        sourceContent += "}\n\n";
    } else if(sm->getRobot()->getDriveHandler()->getMode() == "Mecanum") {
        sourceContent += "float __x, float __y, float __rotation)\n{\n";
        sourceContent += "\tdouble wheelSpeeds[4];\n\n";

        sourceContent += "\twheelSpeeds[RobotDrive::kFrontLeftMotor]  =  __x + __y + __rotation;\n";
        sourceContent += "\twheelSpeeds[RobotDrive::kFrontRightMotor] = -__x + __y - __rotation;\n";
        sourceContent += "\twheelSpeeds[RobotDrive::kRearLeftMotor]   = -__x + __y + __rotation;\n";
        sourceContent += "\twheelSpeeds[RobotDrive::kRearRightMotor]  =  __x + __y - __rotation;\n\n";

        sourceContent += "\tNormalize(wheelSpeeds);\n\n";
        if(FL) sourceContent += "\t" + FL->getName() + "->Set(" + ((FL->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kFrontLeftMotor]);\n";
        if(FR) sourceContent += "\t" + FR->getName() + "->Set(" + ((FR->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kFrontRightMotor]);\n";
        if(BL) sourceContent += "\t" + BL->getName() + "->Set(" + ((BL->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kRearLeftMotor]);\n";
        if(BR) sourceContent += "\t" + BR->getName() + "->Set(" + ((BR->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kRearRightMotor]);\n";
        sourceContent += "}\n\n";

        sourceContent += "void "+sm->getRobot()->getClassName()+"::Normalize(double *wheelSpeeds)\n{\n\t"
                "double maxMagnitude = fabs(wheelSpeeds[0]);\n\t"
                "int i;\n\t"
                "for(i=1; i<4; i++)\n\t{\n\t\t"
                "double temp = fabs(wheelSpeeds[i]);\n\t\t"
                "if(maxMagnitude < temp) maxMagnitude = temp;\n\t}\n"
                "\tif(maxMagnitude > 1.0)\n\t\t"
                "for(i=0; i<4; i++)\n\t\t\t"
                "wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;\n}\n\n";
    } else { //Default to arcade.
        sourceContent += "float __x, float __y)\n{\n\t";
        if(FL) sourceContent += FL->getName() + "->Set(" + ((FL->getInverted() == "%t") ? "-1*" : "") + "(__y+__x));\n\t";
        if(BL) sourceContent += BL->getName() + "->Set(" + ((BL->getInverted() == "%t") ? "-1*" : "") + "(__y+__x));\n\t";
        if(FR) sourceContent += FR->getName() + "->Set(" + ((FR->getInverted() == "%t") ? "-1*" : "") + "(__y-__x));\n\t";
        if(BR) sourceContent += BR->getName() + "->Set(" + ((BR->getInverted() == "%t") ? "-1*" : "") + "(__y-__x));";
        sourceContent += "\n}\n\n";
    }

    foreach(BindGenerator bg, bindings) {
        sourceContent += bg.getFunctionDefinition() + "\n\n";
    }

    sourceContent += "START_ROBOT_CLASS(" + sm->getRobot()->getClassName() + ")\n";

    QFile sourceFile(projectDir.absoluteFilePath("robot.cpp"));
    if(sourceFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("Generating SimpleRobot source...");
        QTextStream of(&sourceFile);
        of << sourceContent.replace('\t',"    ");
    } else {
        qFatal("Could not create robot.cpp! Reason: %s",qPrintable(sourceFile.errorString()));
    }
    sourceFile.close();

    QFile::remove(projectDir.absoluteFilePath("JoystickWrapper.h"));
    QFile::remove(projectDir.absoluteFilePath("JoystickWrapper.cpp"));
    QFile::remove(projectDir.absoluteFilePath("DisplayWrapper.h"));
    QFile::remove(projectDir.absoluteFilePath("DisplayWrapper.cpp"));
    QFile::remove(projectDir.absoluteFilePath("JoystickCallback.h"));
    QFile::copy(":/staticFiles/JoystickWrapper.h",projectDir.absoluteFilePath("JoystickWrapper.h"));
    QFile::copy(":/staticFiles/JoystickWrapper.cpp",projectDir.absoluteFilePath("JoystickWrapper.cpp"));
    QFile::copy(":/staticFiles/DisplayWrapper.h",projectDir.absoluteFilePath("DisplayWrapper.h"));
    QFile::copy(":/staticFiles/DisplayWrapper.cpp",projectDir.absoluteFilePath("DisplayWrapper.cpp"));
    QFile::copy(":/staticFiles/JoystickCallback.h",projectDir.absoluteFilePath("JoystickCallback.h"));

    return true;
}

QString SimpleCPPGenerator::deleteObj(RobotObject *ro)
{
    return "delete " + ro->getName() + ";";
}

QString SimpleCPPGenerator::constructWheel(Wheel *wh)
{
    return wh->getName() + " = new " + wh->getMotorType().mid(1) + "(" + wh->getPort().mid(1) + ", " + wh->getChannel().mid(1) + ");";
}

QString SimpleCPPGenerator::getJoystickBindingStatement(const QString &targetPath)
{
    if(!targetPath.startsWith("$"))
        return "";
    QStringList path = targetPath.mid(1).split('/');
    if(path.size() != 3)
        qFatal("Malformed joystick sender path.");
    QString name = path.first();
    QString button = path.at(1);
    QString action = path.last();
    QString ret = "%1Callback->Set%2Callback(%3,GET_FUNC(%4));";
    if(!button.startsWith("button"))
        qFatal(qPrintable(QString("No such member of joystick: %1").arg(button)));
    button = button.mid(6);
    int btn = button.toInt();
    if(btn > 12 || btn < 1)
        qFatal("Button out of range.");
    if(action.toLower() == "down") action = "Down";
    else if(action.toLower() == "up") action = "Up";
    else if(action.toLower() == "held") action = "Held";
    else qFatal(qPrintable(QString("Joystick button has no event \'%1\'").arg(action)));
    return ret.arg(name).arg(action).arg(btn);
}

Q_EXPORT_PLUGIN2(simplecppgen, SimpleCPPGenerator)
