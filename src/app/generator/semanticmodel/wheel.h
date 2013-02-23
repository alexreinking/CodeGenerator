#ifndef WHEEL_H
#define WHEEL_H

#include "robotobject.h"
#include "../datastructures/exprnodes.h"

class Wheel : public RobotObject
{
public:
    Wheel(const QString &name,
          const QString port, const QString channel,
          const QString isInverted, const QString m,
          bool m_isDefault = false)
              :name(name),port(port),channel(channel),
              isInverted(isInverted),motorType(m),
                m_isDefault(m_isDefault) {}

    int getType() const { return WheelObject; }
    bool isDefault() const { return m_isDefault; }
    QString getName() const { return name; }

    QString getPort() const { return port; }
    QString getChannel() const { return channel; }
    QString getInverted() const { return isInverted; }
    QString getMotorType() const { return motorType; }

private:
    QString name;
    QString port, channel;
    QString isInverted;
    QString motorType;
    bool m_isDefault;
};

#endif // WHEEL_H
