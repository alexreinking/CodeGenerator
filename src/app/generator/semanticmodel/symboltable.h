#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QList>
#include "robotobject.h"

class SymbolTable
{
public:
    SymbolTable();

    RobotObject* getDefaultValue(int typeId);

    void insert(RobotObject* value);
    void remove(QString name);
    RobotObject* getObject(QString name);

private:
    QList<RobotObject*> table;
};

#endif // SYMBOLTABLE_H
