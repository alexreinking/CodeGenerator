#include "drivegenerator.h"

DriveGenerator::DriveGenerator(DriveHandler *dh, const QDir &dir)
{
    className = dh->getMode() + "Drive";
    outDir = dir;
    this->dh = dh;
}

QString DriveGenerator::getClassName() const
{
    return className;
}

bool DriveGenerator::generate()
{
    bool ok = generateDriveClassHeader() && generateDriveClassSource();
#ifdef Q_OS_LINUX
    qDebug("Generated drive class \'%s\'.",qPrintable(className));
#endif
    return ok;
}

bool DriveGenerator::generateDriveClassHeader()
{
    QString out =
            "#ifndef DRIVE_H\n"
            "#define DRIVE_H\n\n"
            "#include \"WPILib.h\"\n\n"
            "class "+className+"\n{\n"
            "public:\n"
            "\t"+className+"();\n"
            "\t~"+className+"();\n\n"
            "\tvoid Drive(float x, float y, float rotation);\n\n"\
            "private:\n\tvoid Normalize(double *wheelSpeeds);\n" + ((dh->getMode() == "Mecanum") ? "\tvoid RotateVector(double &x, double &y, double angle);\n" : "") +
            "\t"+dh->getWheel(DriveHandler::FrontLeft)->getMotorType().mid(1)+"* "+dh->getWheel(DriveHandler::FrontLeft)->getName()+";\n"
            "\t"+dh->getWheel(DriveHandler::FrontRight)->getMotorType().mid(1)+"* "+dh->getWheel(DriveHandler::FrontRight)->getName()+";\n"
            "\t"+dh->getWheel(DriveHandler::BackLeft)->getMotorType().mid(1)+"* "+dh->getWheel(DriveHandler::BackLeft)->getName()+";\n"
            "\t"+dh->getWheel(DriveHandler::BackRight)->getMotorType().mid(1)+"* "+dh->getWheel(DriveHandler::BackRight)->getName()+";\n"
            "};\n\n#endif\n";
    QFile f(outDir.absoluteFilePath("drive.h"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << out;
    } else {
        qFatal("Could not create drive.h! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}

bool DriveGenerator::generateDriveClassSource()
{
    QString out = "#include <cmath>\n#include \"drive.h\"\n\nusing namespace std;\n\n";
    out += className + "::" + className + "()\n{\n";
    out += "\t" + newWheel(dh->getWheel(DriveHandler::FrontLeft)) + "\n";
    out += "\t" + newWheel(dh->getWheel(DriveHandler::FrontRight)) + "\n";
    out += "\t" + newWheel(dh->getWheel(DriveHandler::BackLeft)) + "\n";
    out += "\t" + newWheel(dh->getWheel(DriveHandler::BackRight)) + "\n";
    out += "}\n\n";
    out += className + "::~" + className + "()\n{\n";
    out += "\t" + deleteObj(dh->getWheel(DriveHandler::FrontLeft)) + "\n";
    out += "\t" + deleteObj(dh->getWheel(DriveHandler::FrontRight)) + "\n";
    out += "\t" + deleteObj(dh->getWheel(DriveHandler::BackLeft)) + "\n";
    out += "\t" + deleteObj(dh->getWheel(DriveHandler::BackRight)) + "\n";
    out += "}\n\n";
    out += "void " + className + "::Drive(float x, float y, float rotation)\n{\n";
    out += "\tdouble wheelSpeeds[4];\n\n";
    if(dh->getMode() == "Mecanum") {
        //Here, we would put FOD code.
        out += "\twheelSpeeds[RobotDrive::kFrontLeftMotor] = x + y + rotation;\n";
        out += "\twheelSpeeds[RobotDrive::kFrontRightMotor] = -x + y - rotation;\n";
        out += "\twheelSpeeds[RobotDrive::kRearLeftMotor] = -x + y + rotation;\n";
        out += "\twheelSpeeds[RobotDrive::kRearRightMotor] = x + y - rotation;\n\n";
    } else if(dh->getMode() == "Arcade") { //Default to Arcade Drive.
        out += "\twheelSpeeds[RobotDrive::kFrontLeftMotor] = wheelSpeeds[RobotDrive::kRearLeftMotor] = x + y + rotation;\n";
        out += "\twheelSpeeds[RobotDrive::kFrontRightMotor] = wheelSpeeds[RobotDrive::kRearRightMotor] = -x + y - rotation;\n\n";
    } else if(dh->getMode() == "Tank") {
        out += "\twheelSpeeds[RobotDrive::kFrontLeftMotor] = wheelSpeeds[RobotDrive::kRearLeftMotor] = x;\n";
        out += "\twheelSpeeds[RobotDrive::kFrontRightMotor] = wheelSpeeds[RobotDrive::kRearRightMotor] = y;\n\n";
    }
    out += "\tNormalize(wheelSpeeds);\n\n";
    out += "\t" + dh->getWheel(DriveHandler::FrontLeft)->getName() + "->Set(" + ((dh->getWheel(DriveHandler::FrontLeft)->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kFrontLeftMotor]);\n";
    out += "\t" + dh->getWheel(DriveHandler::FrontRight)->getName() + "->Set(" + ((dh->getWheel(DriveHandler::FrontRight)->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kFrontRightMotor]);\n";
    out += "\t" + dh->getWheel(DriveHandler::BackLeft)->getName() + "->Set(" + ((dh->getWheel(DriveHandler::BackLeft)->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kRearLeftMotor]);\n";
    out += "\t" + dh->getWheel(DriveHandler::BackRight)->getName() + "->Set(" + ((dh->getWheel(DriveHandler::BackRight)->getInverted() == "%t") ? "-1*" : "") + "wheelSpeeds[RobotDrive::kRearRightMotor]);\n";
    out += "}\n\n";

    out += "void "+className+"::Normalize(double *wheelSpeeds)\n{\n\t"
            "double maxMagnitude = fabs(wheelSpeeds[0]);\n\t"
            "int i;\n\t"
            "for(i=1; i<4; i++)\n\t{\n\t\t"
            "double temp = fabs(wheelSpeeds[i]);\n\t\t"
            "if(maxMagnitude < temp) maxMagnitude = temp;\n\t}\n"
            "\tif(maxMagnitude > 1.0)\n\t\t"
            "for(i=0; i<4; i++)\n\t\t\t"
            "wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;\n}\n\n";

    if(dh->getMode() == "Mecanum") {
        out += "void "+className+"::RotateVector(double &x, double &y, double angle)\n{\n\t"
                "double cosA = cos(angle * (3.141592653897/180.0));\n\t"
                "double sinA = sin(angle * (3.141592653897/180.0));\n\t"
                "double xOut = x * cosA - y * sinA;\n\t"
                "double yOut = x * sinA + y * cosA;\n\t"
                "x = xOut;\n\t"
                "y = yOut;\n}\n";
    }

    QFile f(outDir.absoluteFilePath("drive.cpp"));
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream of(&f);
        of << out.replace('\t',"    ");
    } else {
        qFatal("Could not create drive.cpp! Reason: %s",qPrintable(f.errorString()));
    }
    f.close();
    return true;
}

QString DriveGenerator::newDriveGenerator() const
{
    return QString("DriverWrapper* dw = new DriverWrapper(%1);").arg(dh->getMode());
}

QString DriveGenerator::newWheel(Wheel *wh)
{
    return QString("%2 = new %1(%3,%4);").arg(wh->getMotorType().mid(1)).arg(wh->getName()).arg(wh->getPort().mid(1)).arg(wh->getChannel().mid(1));
}

QString DriveGenerator::deleteObj(RobotObject *ro)
{
    return "delete " + ro->getName() + ";";
}
