#ifndef DRIVEGENERATOR_H
#define DRIVEGENERATOR_H

#include <QtCore>
#include <QDebug>

#include <GeneratorPlugin.h>

class DriveGenerator
{
public:
    DriveGenerator(DriveHandler* dh, const QDir &dir);
    QString getClassName() const;
    bool generate();

    QString newDriveGenerator() const;

private:
    bool generateDriveClassHeader();
    bool generateDriveClassSource();
    QString newWheel(Wheel* wh);
    QString deleteObj(RobotObject* ro);

    DriveHandler* dh;
    QString className;
    QDir outDir;
};

#endif // DRIVEGENERATOR_H
