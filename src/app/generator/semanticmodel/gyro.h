#ifndef GYRO_H
#define GYRO_H

#include "robotobject.h"
#include "../datastructures/exprnodes.h"

class Gyro : public RobotObject
{
public:
    Gyro(const QString &name,const int channel,
          bool m_isDefault = false)
              :name(name),channel(channel),
                m_isDefault(m_isDefault) {}

    int getType() const { return GyroObject; }
    bool isDefault() const { return m_isDefault; }
    QString getName() const { return name; }

    int getChannel() const { return channel; }

private:
    QString name;
    int channel;
    bool m_isDefault;
};

#endif // GYRO_H
