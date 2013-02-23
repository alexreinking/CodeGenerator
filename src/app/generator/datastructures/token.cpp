#include "token.h"

Token::Token()
{
    lineNumber = -1;
    linePos = -1;
    value = "";
    myType = Token::Invalid;
}

Token::Token(int ln, int lp, TokenType t, const QString &val)
{
    lineNumber = ln;
    linePos = lp;
    value = val;
    myType = t;
}

QString Token::print()
{
    QString tmp = value;
    tmp.replace('\t',"\\t");
    tmp.replace('\n',"\\n");
    tmp = tmp.trimmed();

    QString typeName;
    switch(myType) {
    case Token::Invalid:
        typeName = "Invalid";
        break;
    case Token::Keyword:
        typeName = "Keyword";
        break;
    case Token::Number:
        typeName = "Number";
        break;
    case Token::Tab:
        typeName = "Tab";
        break;
    case Token::ForeignBlock:
        typeName = "ForeignBlock";
        break;
    case Token::String:
        typeName = "String";
        break;
    case Token::Break:
        typeName = "Break";
        break;
    case Token::Identifier:
        typeName = "Identifier";
        break;
    case Token::Operator:
        typeName = "Operator";
        break;
    case Token::BlockOpen:
        typeName = "[Block Open]";
        break;
    case Token::BlockClose:
        typeName = "[Block Close]";
        break;
    default:
        typeName = "ERROR";
        break;
    }

    // \E[0m = reset color.
    // \E[1;32m = green text, no bg change.
    // \E[1;36m = cyan text, no bg change
    return QString("Token of type \E[1;32m%1\E[0m at line %2 char %3: \E[30;47m%4\E[0m")
            .arg(typeName).arg(lineNumber).arg(linePos).arg(tmp);
}
