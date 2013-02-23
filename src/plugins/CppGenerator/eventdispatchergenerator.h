#ifndef EVENTDISPATCHERGENERATOR_H
#define EVENTDISPATCHERGENERATOR_H

#include <QtCore>
#include <QDebug>

#include <GeneratorPlugin.h>

class EventDispatcherGenerator
{
public:
    EventDispatcherGenerator(Robot* r, const QDir &dir);

    bool generate();
    void setTeleopClassName(const QString &str) { teleopClassName = str; }

    void addJoystick(Joystick *joy);

private:
    bool generateEDHeader();
    bool generateEDSource();

    QVector<Joystick*> joys;

    Robot* m_robot;
    QDir outDir;
    QString teleopClassName;
};

#endif // EVENTDISPATCHERGENERATOR_H
