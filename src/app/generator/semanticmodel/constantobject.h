#ifndef CONSTANTOBJECT_H
#define CONSTANTOBJECT_H

#include "robotobject.h"

class Constant : public RobotObject
{
public:
    Constant(QString name, double value) { this->name = name; this->value = value; }

    int getType() const { return ConstantObject; }
    bool isDefault() const { return false; }
    QString getName() const { return name; }

    double getValue() const  { return value; }

private:
    QString name;
    double value;
};

#endif // CONSTANTOBJECT_H
