#ifndef ROBOT_H
#define ROBOT_H

#include <QString>
#include <QList>
#include "robotobject.h"
#include "drivehandler.h"

class Robot
{
public:
    Robot(const QString &cName)
    {
        className = cName;
        this->dh = 0;
    }

    void setDriveHandler(DriveHandler* dh) { this->dh = dh; }
    DriveHandler* getDriveHandler() const { return dh; }

    void addObject(RobotObject* ro) { if(!objects.contains(ro) && ro) objects.append(ro); }
    void removeObject(RobotObject* ro) { objects.removeOne(ro); }
    QList<RobotObject*> getObjects() const { return objects; }

    QString getClassName() const { return className; }

protected:
    QString className;
    DriveHandler* dh;
    QList<RobotObject*> objects;
};

#endif // ROBOT_H
