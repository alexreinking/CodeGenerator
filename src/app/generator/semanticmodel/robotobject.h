#ifndef ROBOTOBJECT_H
#define ROBOTOBJECT_H

#include <QStringList>

class RobotObject
{
public:
    enum DataType {
        WheelObject, JoystickObject, ConstantObject, IndicatorObject, GyroObject, BindingObject,
        UserType
    };

    virtual ~RobotObject() {}
    virtual int getType() const = 0;
    virtual bool isDefault() const = 0;
    virtual QString getName() const = 0;
};

#endif // ROBOTOBJECT_H
