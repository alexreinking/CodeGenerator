#ifndef SEMANTICBUILDER_H
#define SEMANTICBUILDER_H

#include "../datastructures/ast.h"
#include "../datastructures/frcnodes.h"
#include "robotobjects.h"
#include "symboltable.h"
#include "semanticmodel.h"

class SemanticBuilder
{
public:
    SemanticBuilder(AbstractSyntaxTree* ast = 0);
    SemanticModel* buildSemanticModel();

private:
    AbstractSyntaxTree* ast;
    SymbolTable* symTable;

    Robot* buildRobot(RobotNode* robot);
    RobotObject* buildFromHas(HasNode* has);
    Wheel* buildWheel(HasNode* hasWheel);
    Joystick* buildJoystick(HasNode* hasJoystick);
    Gyro* buildGyro(HasNode* hasGyro);
    DriveHandler* buildDriveHandler(DrivesNode* drives);
    RobotIndicator* buildIndicator(IndicateNode* ind);
    RobotBinding* buildBinding(BindNode* bind);

    QString evaluateExpression(ExpressionNode* expr);
    double numberFromExpressionString(const QString &str, bool *ok = 0);
};

#endif // SEMANTICBUILDER_H
