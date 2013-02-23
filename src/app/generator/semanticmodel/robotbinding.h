#ifndef ROBOTBINDING_H
#define ROBOTBINDING_H

#include "robotobject.h"

class RobotBinding : public RobotObject
{
public:
    RobotBinding(const QString &sender, const QString &receiver, bool foreignCode = false)
        : sender(sender), receiver(receiver), foreignCode(foreignCode) {}

    int getType() const { return BindingObject; }
    bool isDefault() const { return false; }
    QString getName() const { return ""; }

    QString getSender() const { return sender; }
    QString getReceiver() const { return receiver; }
    bool getForeignCode() const { return foreignCode; }

    void setSender(const QString &sender) { this->sender = sender; }
    void setReceiver(const QString &receiver) { this->receiver = receiver; }
    void setForeignCode(bool foreignCode) { this->foreignCode = foreignCode; }

private:
    QString sender;
    QString receiver;
    bool foreignCode;
};


#endif // ROBOTBINDING_H
