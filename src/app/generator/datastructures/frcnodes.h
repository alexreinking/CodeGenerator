#ifndef FRCNODES_H
#define FRCNODES_H

#include <QHash>
#include <QStringList>
#include "../datastructures/treenode.h"
#include "exprnodes.h"
typedef QHash<QString,ExpressionNode*> OptionsStruct;
QString printOpts(OptionsStruct opt);

enum FRCNodeType { Robot_t, Has, Drives, Indicate, Bind, Module, Action, Instruction };

//Composite
class RobotNode : public TreeNode
{
public:
    RobotNode(const QString &name) { this->name = name; }
    ~RobotNode() {
        foreach(TreeNode* t, children) {
            delete t;
        }
    }

    //Interface definitions.
    QList<TreeNode*> getChildren() const { return children; }
    void addChild(TreeNode* child) { child->setParent(this); children << child; }
    QString toString() const {
        QString ret("Robot \"%1\" has %2 children:");
        ret = ret.arg(name).arg(children.length());
        foreach(TreeNode* node, children) {
            ret += "\n\t" + node->toString();
        }
        return ret;
    }

    //RobotNode-specific functions.
    QString getName() const { return name; }

    int getType() const { return Robot_t; }

private:
    QString name;
    QList<TreeNode*> children;
};

//Composite
class ModuleNode : public TreeNode
{
public:
    ModuleNode(const QString &name) { this->name = name; }
    ~ModuleNode() {
        foreach(TreeNode* t, children)
            delete t;
    }

    QList<TreeNode*> getChildren() const { return children; }
    void addChild(TreeNode* child) { child->setParent(this); children << child; }
    QString toString() const {
        QString ret("Module \"%1\" has %2 children:");
        ret = ret.arg(name).arg(children.length());
        foreach(TreeNode* node, children) {
            ret += "\n\t" + node->toString();
        }
        return ret;
    }

    QStringList getParameters() const { return parameters; }
    void setParameters(QStringList params) { parameters = params; }

    QString getName() const { return name; }

    int getType() const { return Module; }

private:
    QString name;
    QStringList parameters;
    QList<TreeNode*> children;
};

//Composite of InstructionNodes and HasNodes
class ActionNode : public TreeNode
{
public:
    ActionNode(TreeNode* parent, const QString &name, QString parameters)
        : parent(parent), name(name), parameters(parameters) {}
    ~ActionNode() {}

    TreeNode *getParent() const { return parent; }
    void setParent(TreeNode *n) { parent = n; }
    QString toString() const {
        return QString("Action \'%1\' with %2 instruction(s).").arg(name).arg(children.size());
    }

    int getType() const { return Action; }

    QString getName() const { return name; }
    QString getParameters() const { return parameters; }

    QList<TreeNode*> getChildren() const { return children; }
    void addChild(TreeNode *child) { child->setParent(this); children << child; }

private:
    TreeNode* parent;
    QString name;
    QString parameters;
    QList<TreeNode*> children;
};

//Leaf
class InstructionNode : public TreeNode
{
public:
    InstructionNode(TreeNode* parent, const QString &object, const QString &methodName, OptionsStruct parameters)
        : parent(parent),object(object),methodName(methodName),parameters(parameters)
    { }
    ~InstructionNode() {}

    TreeNode *getParent() const { return parent; }
    void setParent(TreeNode *n) { parent = n; }
    QString toString() const {
        QString ret("Instruction node: Calls %1 on %2 with parameters: %3.");
        return ret.arg(methodName).arg(object).arg(printOpts(parameters));
    }

    int getType() const { return Instruction; }

    QString getObject() const { return object; }
    QString getMethodName() const { return methodName; }
    OptionsStruct getParameters() const  { return parameters; }

private:
    TreeNode* parent;
    QString object;
    QString methodName;
    OptionsStruct parameters;
};

//Leaf
class HasNode : public TreeNode
{
public:
    HasNode(TreeNode* parent, const QString &typeName, const QString &name, OptionsStruct opts, bool isDefault = false)
    {
        this->name = name;
        this->typeName = typeName;
        this->opts = opts;
        this->isDefault = isDefault;
        p = parent;
    }
    ~HasNode() {}

    //Interface definitions.
    TreeNode* getParent() const { return p; }
    void setParent(TreeNode *n) { p = n; }
    QString toString() const {
        QString ret("Has object \"%1\" of type \"%2\".%3 Its options are: %4");
        ret = ret.arg(name).arg(typeName).arg(isDefault ? " It is the default value for its type." : "").arg(printOpts(opts));
        return ret;
    }

    //HasNode-specific functions.
    QString getName() const { return name; }
    QString getTypeName() const { return typeName; }
    bool getDefault() const { return isDefault; }
    OptionsStruct getOptions() const { return opts; }

    int getType() const { return Has; }

private:
    TreeNode *p;
    QString name;
    QString typeName;
    OptionsStruct opts;
    bool isDefault;
};

//Leaf
class DrivesNode : public TreeNode
{
public:
    DrivesNode(TreeNode* parent, const QString &driveType, OptionsStruct opts)
    {
        dt = driveType;
        this->opts = opts;
        p = parent;
    }
    ~DrivesNode() {}

    //Interface definitions.
    TreeNode* getParent() const { return p; }

    //DrivesNode-specific functions.
    QString getDriveType() const { return dt; }
    OptionsStruct getOptions() const { return opts; }
    QString toString() const {
        QString ret("Robot drives using module \"%1\". The module is configured by: %2");
        ret = ret.arg(dt).arg(printOpts(opts));
        return ret;
    }

    int getType() const { return Drives; }

private:
    OptionsStruct opts;
    QString dt;
    TreeNode *p;
};

class IndicateNode : public TreeNode
{
public:
    IndicateNode(TreeNode* parent, const QString &foreignCode, OptionsStruct opts)
    {
        p = parent;
        code = foreignCode;
        this->opts = opts;
    }
    ~IndicateNode() {}

    //Interface definitions.
    TreeNode* getParent() const { return p; }

    QString getCode() const { return code; }
    OptionsStruct getOptions() const { return opts; }

    QString toString() const { return code; }
    int getType() const { return Indicate; }

private:
    QString code;
    OptionsStruct opts;
    TreeNode *p;
};

class BindNode : public TreeNode
{
public:
    BindNode(TreeNode* parent, const QString &sender, const QString &receiver, bool foreignCode = false, OptionsStruct parameters = OptionsStruct())
        : parent(parent),sender(sender),receiver(receiver),foreignCode(foreignCode),parameters(parameters) { }

    TreeNode* getParent() const { return parent; }
    QString toString() const { return QString("Binding %1 to %2.").arg(sender).arg(receiver); }
    int getType() const { return Bind; }

    bool isForeignCode() const { return foreignCode; }
    QString getSender() const { return sender; }
    QString getReceiver() const { return receiver; }
    OptionsStruct getParameters() const { return parameters; }

private:
    TreeNode* parent;
    QString sender;
    QString receiver;
    bool foreignCode;
    OptionsStruct parameters;
};

#endif // FRCNODES_H
