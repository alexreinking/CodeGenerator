#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "robotobject.h"

class Joystick : public RobotObject
{
public:
    enum JoystickType { Extreme3D, Attack3 };

    Joystick(const QString &name, int port,
             JoystickType t = Extreme3D, bool m_isDefault = false, double exp = 0.0)
        :port(port),name(name),joyType(t),m_isDefault(m_isDefault),exponent(exp) {}

    int getType() const { return JoystickObject; }
    bool isDefault() const { return m_isDefault; }
    QString getName() const { return name; }

    JoystickType getJoystickType() const { return joyType; }
    int getPort() const { return port; }
    double getExponent() const { return exponent; }

private:
    int port;
    QString name;
    JoystickType joyType;
    bool m_isDefault;
    double exponent;
};

#endif // JOYSTICK_H
