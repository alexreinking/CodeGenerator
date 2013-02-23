#include "symboltable.h"

SymbolTable::SymbolTable()
{
}

RobotObject* SymbolTable::getDefaultValue(int typeId)
{
    foreach(RobotObject* obj, table) {
        if(obj->isDefault() && obj->getType() == typeId)
            return obj;
    }
    return 0;
}

void SymbolTable::insert(RobotObject *value)
{
    if(!table.contains(value))
        table.append(value);
}

void SymbolTable::remove(QString name)
{
    for(int i = 0; i < table.length(); i++) {
        if(table.at(i)->getName() == name) {
            table.removeAt(i);
            return;
        }
    }
}

RobotObject* SymbolTable::getObject(QString name) {
    foreach(RobotObject* obj, table) {
        if(obj->getName() == name)
            return obj;
    }
    return 0;
}
