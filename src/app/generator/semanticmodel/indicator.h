#ifndef INDICATOR_H
#define INDICATOR_H

#include "robotobject.h"

class RobotIndicator : public RobotObject
{
public:
    RobotIndicator(const QString &code)
    {
        static int l = 1;
        this->code = code;
        name = "";
        fmt = "%f";
        line = l++;
    }

    int getType() const { return IndicatorObject; }
    bool isDefault() const { return false; }
    QString getName() const { return name; }

    int getLine() const { return line; }
    QString getCode() const { return code; }
    QString getFormatString() const { return fmt; }

    void setName(const QString &name) { this->name = name; }
    void setFormat(const QString &fmt) { this->fmt = fmt; }

private:
    QString code;
    QString name;
    QString fmt;
    int line;
};

#endif // INDICATOR_H
