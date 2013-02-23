#include <QtDebug>
#include "parser.h"

Parser::Parser(QList<Token> input)
{
    tokens = input;
    parseResultHasChanged = true;
    parseResult = 0;
    pos = 0;
}

AbstractSyntaxTree* Parser::parse()
{
    if(!parseResultHasChanged)
        return parseResult;
    parseResult = new AbstractSyntaxTree;
    for(pos = 0; pos < tokens.size(); pos++) {
        if(tokens.at(pos).getType() == Token::Keyword) {
            if(tokens.at(pos).getValue() == "Robot") {
                parseResult->setRobotNode(parseRobotNode());
            } else if(tokens.at(pos).getValue() == "Module") {
                parseResult->addModuleNode(parseModuleNode());
            }
        }
    }
    parseResultHasChanged = false;
    return parseResult;
}

QString Parser::printTree() const
{
    QString ret("\E[1mTrees exist.\E[0m Trees are \E[33mbrown\E[0m and \E[32mgreen\E[0m. This class builds data structures, though.\n"
                   "Do data structures have color? I'm not sure.\n"
                   "\E[1;34mFun fact:\E[0m This sentence is \E[31mc\E[1;31mo\E[1;33ml\E[32mo\E[34mr\E[1;34me\E[35md\E[0m if you\'re running bash...\n"
                   "At any rate, here\'s the tree I made for you :)\n");
    ret += parseResult->getRobotNode()->toString();
    return ret;
}

void Parser::setInput(QList<Token> input)
{
    parseResultHasChanged = true;
    tokens = input;
}

OptionsStruct Parser::parseOptionsArray()
{
    if(tokens.at(pos).getType() != Token::Operator && tokens.at(pos).getValue() != "[")
        qFatal("Expected token: [");
    QString expVal;
    pos++;
    OptionsStruct ret;
    while(expVal = "", tokens.at(pos).getValue() != "]") {
        if(tokens.at(pos).getType() != Token::Identifier) {
            qFatal("%s",qPrintable(QString("Expected identifier at line %1, column %2.").arg(tokens.at(pos).getLineNumber()).arg(tokens.at(pos).getLinePos())));
        }
        QString optName = tokens.at(pos++).getValue();
        if(tokens.at(pos).getValue() != ":") {
            qFatal("%s",qPrintable(QString("Expected colon at line %1, column %2.").arg(tokens.at(pos).getLineNumber()).arg(tokens.at(pos).getLinePos())));
        }
        pos++;
        ExpressionNode* expr = parseExpression();
        if(tokens.at(pos).getValue() == ",")
            pos++;
        else if(tokens.at(pos).getValue() == "]") ;
        else
            qFatal("Expected token , or ].");
        ret.insert(optName,expr);
    }
    pos++;
    return ret;
}

RobotNode* Parser::parseRobotNode()
{
    if(tokens.at(pos++).getValue() != "Robot")
        qFatal("Parser: parseRobotNode() erroneously called.");
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Expected name after Robot declaration.");

    RobotNode* ret = new RobotNode(tokens.at(pos++).getValue());
    if(tokens.at(++pos).getType() != Token::BlockOpen)
        qFatal("Robot declaration cannot be blank.");
    pos++;
    while(tokens.at(pos).getType() == Token::Keyword) {
        if(tokens.at(pos).getValue() == "Has") {
            ret->addChild(parseHasNode());
        } else if(tokens.at(pos).getValue() == "Drives") {
            ret->addChild(parseDrivesNode());
        } else if(tokens.at(pos).getValue() == "Indicate") {
            ret->addChild(parseIndicateNode());
        } else if(tokens.at(pos).getValue() == "Bind") {
            ret->addChild(parseBindNode());
        } else {
            qFatal("Unimplemented feature: %s",qPrintable(tokens.at(pos).getValue()));
        }
    }
    return ret;
}

HasNode* Parser::parseHasNode()
{
    if(tokens.at(pos++).getValue() != "Has")
        qFatal("Parser: parseHasNode() erroneously called.");
    bool isDefault = false;
    QString typeName;
    QString objName;
    OptionsStruct opts;
    if(tokens.at(pos).getType() == Token::Keyword && tokens.at(pos).getValue() == "default") {
        isDefault = true;
        pos++;
    }
    if(tokens.at(pos).getType() == Token::Identifier)
        typeName = tokens.at(pos++).getValue();
    else
        qFatal("Expected first identifier in Has expression.");
    if(tokens.at(pos).getType() == Token::Identifier)
        objName = tokens.at(pos++).getValue();
    else
        qFatal("Expected second identifier in Has expression.");
    opts = parseOptionsArray();
    if(tokens.at(pos++).getType() != Token::Break)
        qFatal("Malformed Has expression. No statements should come after the list of parameters.");
    return new HasNode(0,typeName,objName,opts,isDefault);
}

DrivesNode* Parser::parseDrivesNode()
{
    if(tokens.at(pos++).getValue() != "Drives")
        qFatal("Parser: parseDrivesNode() erroneously called.");
    QString moduleName;
    OptionsStruct opts;
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Expected identifier after \'Drives\'.");
    moduleName = tokens.at(pos++).getValue();
    opts = parseOptionsArray();
    if(tokens.at(pos++).getType() != Token::Break)
        qFatal("Malformed Drives expression. No statements should come after the list of parameters.");
    return new DrivesNode(0,moduleName,opts);
}

IndicateNode* Parser::parseIndicateNode()
{
    OptionsStruct opts;
    if(tokens.at(pos++).getValue() != "Indicate")
        qFatal("Parser: parseIndicateNode() erroneously called.");
    QString code;
    if(tokens.at(pos).getType() != Token::ForeignBlock)
        qFatal("Expected native code after Indicate!");
    code = tokens.at(pos++).getValue();
    if(tokens.at(pos).getType() == Token::Operator && tokens.at(pos).getValue() == "[") {
        opts = parseOptionsArray();
    }
    if(tokens.at(pos++).getType() != Token::Break)
        qFatal("Malformed Indicate expression. No statements should come after the options array.");
    return new IndicateNode(0,code,opts);
}

BindNode* Parser::parseBindNode()
{
    if(tokens.at(pos++).getValue() != "Bind")
        qFatal("Parser: parseBindNode() erroneously called.");
    QString sender, receiver;
    bool foreignCode = false;
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Expected identifier for sender.");
    sender = parseExpressiveIdentifier();
    if(tokens.at(pos).getType() == Token::Identifier)
        receiver = parseExpressiveIdentifier();
    else if(tokens.at(pos).getType() == Token::ForeignBlock) {
        foreignCode = true;
        receiver = tokens.at(pos++).getValue();
    } else {
        qFatal("Expected identifier or native code for receiver.");
    }
    OptionsStruct opts;
    if(tokens.at(pos).getType() == Token::Operator && tokens.at(pos).getValue() == "[")
        opts = parseOptionsArray();
    if(tokens.at(pos++).getType() != Token::Break)
        qFatal("Malformed Bind expression. No statements should come after the receiver/parameter list.");
    return new BindNode(0,sender,receiver,foreignCode,opts);
}

ModuleNode* Parser::parseModuleNode()
{
    if(tokens.at(pos++).getValue() != "Module")
        qFatal("Parser: parseModuleNode() erroneously called.");
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Expected name after Robot declaration.");

    ModuleNode* ret = new ModuleNode(tokens.at(pos++).getValue());
    if(tokens.at(++pos).getType() != Token::BlockOpen) //Skip Break
        qFatal("Robot declaration cannot be blank.");
    pos++; //Skip Block open
    while(tokens.at(pos).getType() == Token::Keyword) {
        if(tokens.at(pos).getValue() == "Has") {
            ret->addChild(parseHasNode());
        } else if(tokens.at(pos).getValue() == "Action") {
            ret->addChild(parseActionNode());
        } else {
            qFatal("Parser: Unimplemented feature: %s",qPrintable(tokens.at(pos).getValue()));
        }
    }
    return ret;
}

ActionNode* Parser::parseActionNode()
{
    if(tokens.at(pos++).getValue() != "Action")
        qFatal("Parser: parseActionNode() erroneously called.");
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Expected name after action declaration.");
    QString name = tokens.at(pos).getValue();
    QString methodSignature = "";
    pos++; //Next token. Either * or [break]
    while(tokens.at(pos).getType() != Token::Break) //Everything after is considered part of the signature.
    {
        methodSignature += tokens.at(pos).getValue() + " ";
        pos++;
    } //Ends at break;
    pos++; //Skip break;

    ActionNode* ret = new ActionNode(0,name,methodSignature);

    if(tokens.at(pos++).getType() != Token::BlockOpen)
        qFatal("Parser: Actions cannot be blank.");
    while(tokens.at(pos).getType() != Token::BlockClose) {
        if(tokens.at(pos).getValue() == "Has")
            ret->addChild(parseHasNode());
        else if(tokens.at(pos).getType() == Token::Identifier)
            ret->addChild(parseInstructionNode());
        else
            qFatal(qPrintable(QString("Parser: Unexpected token %1").arg(tokens.at(pos).getValue())));
    }
    pos++;
    return ret;
}

InstructionNode* Parser::parseInstructionNode()
{
    if(tokens.at(pos).getType() != Token::Identifier)
        qFatal("Parser: Missing actor in instruction.");
    QString actor = "$" + tokens.at(pos++).getValue();
    QString methodName = "";
    if(tokens.at(pos).getValue() != ".")
        qFatal("Parser: Object present, but no method specified.");
    while(tokens.at(pos).getValue() == ".")
    {
        pos++;
        if(!methodName.isEmpty())
            actor += "/" + methodName;
        methodName = tokens.at(pos).getValue();
        pos++;
    }
    if(tokens.at(pos).getType() != Token::Operator && tokens.at(pos).getValue() != "[")
        qFatal("Parser: Expected options array after actor in instruction.");
    OptionsStruct opts = parseOptionsArray();
    InstructionNode* ret = new InstructionNode(0,actor,methodName,opts);
    if(tokens.at(pos++).getType() != Token::Break)
        qFatal("Malformed instruction. No statements should come after the options array.");
    return ret;
}

ExpressionNode* Parser::parseExpression()
{
    ExpressionNode *top = parseEqualityLogic();
    while(pos < tokens.size())
    {
        Token op = tokens.at(pos);
        if(op.getValue() == "or" || op.getValue() == "and")
            pos++;
        else
            return top;
        top = new BinaryOperation(op.getValue(),top,parseEqualityLogic());
    }
    return top;
}

ExpressionNode* Parser::parseEqualityLogic()
{
    ExpressionNode *top = parseAddition();
    while(pos < tokens.size())
    {
        Token op = tokens.at(pos);
        if((QList<QString>() << "eq" << "ne" << "lt" << "le" << "gt" << "ge").contains(op.getValue()))
            pos++;
        else
            return top;
        top = new BinaryOperation(op.getValue(),top,parseAddition());
    }
    return top;
}

ExpressionNode* Parser::parseAddition()
{
    ExpressionNode *top = parseMultiplication();
    while(pos < tokens.size())
    {
        Token op = tokens.at(pos);
        if((QList<QString>() << "+" << "-").contains(op.getValue()))
            pos++;
        else
            return top;
        top = new BinaryOperation(op.getValue(),top,parseMultiplication());
    }
    return top;
}

ExpressionNode* Parser::parseMultiplication()
{
    ExpressionNode *top = parseFactor();
    while(pos < tokens.size())
    {
        Token op = tokens.at(pos);
        if((QList<QString>() << "*" << "/" << "%").contains(op.getValue()))
            pos++;
        else
            return top;
        top = new BinaryOperation(op.getValue(),top,parseFactor());
    }
    return top;
}

ExpressionNode* Parser::parseFactor()
{
    ExpressionNode *top = 0;
    bool isNegative = false, isNegated = false;
    while((QList<QString>() << "-" << "not").contains(tokens.at(pos).getValue())) { //Order doesn't matter.
        if(tokens.at(pos).getValue() == "-")
            isNegative = !isNegative;
        if(tokens.at(pos).getValue() == "not")
            isNegated = !isNegated;
        pos++;
    }
    if(tokens.at(pos).getType() == Token::Number) {
        top = new TerminalValue("#" + tokens.at(pos++).getValue(),0);
    } else if(tokens.at(pos).getType() == Token::Identifier) {
        top = new TerminalValue(parseExpressiveIdentifier(),0);
    } else if(tokens.at(pos).getType() == Token::String) {
        top = new TerminalValue("@" + tokens.at(pos++).getValue(),0);
    } else if(tokens.at(pos).getValue() == "true") {
        top = new TerminalValue("%t",0);
        pos++;
    } else if(tokens.at(pos).getValue() == "false") {
        top = new TerminalValue("%f",0);
        pos++;
    } else if(tokens.at(pos).getValue() == "(") {
        pos++;
        top = parseExpression();
        if(tokens.at(pos).getValue() != ")") {
            qFatal("%s",qPrintable(QString("Malformed expression at around line %1, col %2. Possibly mismatched parenthesis.")
                              .arg(tokens.at(pos).getLineNumber()).arg(tokens.at(pos).getLinePos())));
        } pos++;
    }
    if(isNegative) {
        top = new UnaryOperation("-",top);
    }
    if(isNegated) {
        top = new UnaryOperation("not",top);
    }
    return top;
}

QString Parser::parseExpressiveIdentifier()
{
    QString ret = tokens.at(pos++).getValue();
    while(tokens.at(pos).getValue() == ".") {
        pos++;
        if(tokens.at(pos).getType() == Token::Identifier) {
            ret += "/" + tokens.at(pos).getValue();
        } else {
            qFatal("%s",qPrintable(QString("Malformed identifier phrase at line %1, col %2. Expected identifier after accessor.")
                              .arg(tokens.at(pos).getLineNumber()).arg(tokens.at(pos).getLinePos())));
        }
        pos++;
    }
    return "$" + ret;
}
