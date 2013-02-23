#ifndef EXPRNODES_H
#define EXPRNODES_H

//Expression components.
/*
Note: Expression nodes are NOT tree nodes. These are fully owned by a TreeNode.
This is because they are not fully-qualified statements in the language,
*/

#include <QList>
#include <QString>

enum ExpressionNodeType {
    Expression, Unary, Binary, Terminal
};

class ExpressionNode
{
public:
    ExpressionNode(ExpressionNode* parent = 0)
    {
        this->parent = parent;
        colors << "\E[31m" << "\E[1;31m" << "\E[1;33m" << "\E[32m" << "\E[34m" << "\E[1;34m" << "\E[35m";
    }
    virtual ExpressionNode* getParent() const { return parent; }
    virtual int getType() const { return Expression; }
    virtual QString toString(int colorIndex = 0) const = 0;

protected:
    QList<QString> colors;

private:
    ExpressionNode* parent;
};


class UnaryOperation : public ExpressionNode
{
public:
    UnaryOperation(const QString &op, ExpressionNode* operand, ExpressionNode* parent = 0):ExpressionNode(parent),m_oper(operand),m_op(op) {}
    QString getOperator() const { return m_op; }
    ExpressionNode* getOperand() const { return m_oper; }

    int getType() const { return Unary; }
    QString toString(int colorIndex = 0) const {
        return QString("%1"+colors[colorIndex]+"(\E[0m%2"+colors[colorIndex]+")\E[0m")
                .arg(m_op)
                .arg(m_oper->toString((colorIndex + 1)%7));
    }

private:
    ExpressionNode* m_oper;
    QString m_op;
};

class BinaryOperation : public ExpressionNode
{
public:
    BinaryOperation(const QString &op, ExpressionNode* left, ExpressionNode* right, ExpressionNode* parent = 0)
        : ExpressionNode(parent),m_left(left),m_right(right),m_op(op) {}

    QString getOperator() const { return m_op; }
    ExpressionNode* getLeft() const { return m_left; }
    ExpressionNode* getRight() const { return m_right; }

    int getType() const { return Binary; }
    QString toString(int colorIndex = 0) const {
        return QString(colors[colorIndex]+"(\E[0m%1"+colors[colorIndex]+")\E[0m %2 "+colors[colorIndex]+"(\E[0m%3"+colors[colorIndex]+")\E[0m")
                .arg(m_left->toString((colorIndex+1)%7))
                .arg(m_op)
                .arg(m_right->toString((colorIndex+1)%7));
    }

private:
    ExpressionNode* m_left;
    ExpressionNode* m_right;
    QString m_op;
};

class TerminalValue : public ExpressionNode
{
public:
    TerminalValue(QString val, ExpressionNode* parent = 0):ExpressionNode(parent),value(val) {}
    QString getValue() const { return value; }

    int getType() const { return Terminal; }
    QString toString(int colorIndex = 0) const { Q_UNUSED(colorIndex) return value; }

private:
    QString value; //Can hold a number: 1.2. Can hold a name: Extreme3DPro. Can hold a reference: joy1.x
    //Syntax in each case: "#1.2", "$Extreme3DPro", "$joy1/x"
    //The "$" signifies an identifier.
    //The "#" signifies a numerical constant.
    //The "%" signifies a built-in constant (pi, t, f)
};

#endif // EXPRNODES_H
