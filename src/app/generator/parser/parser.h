#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include <QStack>
#include "../datastructures/token.h"
#include "../datastructures/ast.h"
#include "../datastructures/frcnodes.h"
#include "../interfaces/parsinghelperinterface.h"

class Parser
{
public:
    Parser(QList<Token> input);
    AbstractSyntaxTree* parse(); //GIVES ownership of AST to caller.

    QString printTree() const;
    void setInput(QList<Token> input);

private:
    AbstractSyntaxTree* parseResult;
    bool parseResultHasChanged;
    QList<Token> tokens; int pos;
    QList<ParsingHelperInterface*> helpers;

    OptionsStruct parseOptionsArray();

    RobotNode* parseRobotNode();
    HasNode* parseHasNode();
    DrivesNode* parseDrivesNode();
    IndicateNode* parseIndicateNode();
    BindNode* parseBindNode();

    ModuleNode* parseModuleNode();
    ActionNode* parseActionNode();
    InstructionNode* parseInstructionNode();

    ExpressionNode* parseExpression(); //or and
    ExpressionNode* parseEqualityLogic(); //eq gt lt ge le ne
    ExpressionNode* parseAddition(); //+ -
    ExpressionNode* parseMultiplication(); //* / %
    ExpressionNode* parseFactor(); //- not
    QString parseExpressiveIdentifier();
};

#endif // PARSER_H
