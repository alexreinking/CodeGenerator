#ifndef DRIVEHANDLER_H
#define DRIVEHANDLER_H

#include <QList>
#include "wheel.h"
#include "joystick.h"

class DriveHandler
{
public:
    enum WheelRole { FrontLeft=0, FrontRight=1, BackLeft=2, BackRight=3 };
    enum JoystickRole { PrimaryJoystick=0, SecondaryJoystick=1 };

    DriveHandler(const QString &mode)
    {
        for(int i = 0; i < 4; i++)
            wheels[i] = 0;
        for(int i = 0; i < 2; i++)
            joysticks[i] = 0;
        this->mode = mode;
    }

    void setWheel(Wheel* wh, WheelRole role)
    {
        wheels[role] = wh;
    }

    Wheel* getWheel(WheelRole role) const
    {
        if(!wheels[role])
            qCritical("Not all wheels specified in DriveHandler!");
        return wheels[role];
    }

    void setJoystick(Joystick* joy, JoystickRole role)
    {
        joysticks[role] = joy;
    }

    Joystick* getJoystick(JoystickRole role) const
    {
        return joysticks[role];
    }

    QString getMode() const
    {
        return mode;
    }

    void setMode(const QString &m)
    {
        mode = m;
    }


protected:
    QString mode;
    Wheel* wheels[4];
    Joystick* joysticks[2];
};

#endif // DRIVEHANDLER_H
