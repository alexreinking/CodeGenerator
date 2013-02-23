#ifndef TELEOPGENERATOR_H
#define TELEOPGENERATOR_H

#include <QtCore>
#include <GeneratorPlugin.h>

class TeleopGenerator
{
public:
    TeleopGenerator(Robot* r, const QDir &dir);
    void setDriveClassName(const QString &dcn);
    QString getClassName() const { return className; }

    bool generate();

private:
    bool generateTeleopHeader();
    bool generateTeleopSource();

    Robot* robot;
    QDir projectDir;
    QString driveClassName;
    QString className;
};

#endif // TELEOPGENERATOR_H
