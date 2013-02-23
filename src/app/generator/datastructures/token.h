#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

class Token
{
public:
    enum TokenType { Invalid, Keyword, Number, Tab, ForeignBlock,
                     String, Break, Identifier, Operator, BlockOpen, BlockClose
                   };

    Token();
    Token(int ln, int lp, TokenType t, const QString &val);

    void setLineNumber(int ln)        { lineNumber = ln; }
    void setLinePos(int lp)           { linePos = lp; }
    void setValue(const QString &val) { value = val; }
    void setType(TokenType t)         { myType = t; }

    int       getLineNumber() const { return lineNumber; }
    int       getLinePos()    const { return linePos; }
    QString   getValue()      const { return value; }
    TokenType getType()       const { return myType; }

    QString print();

private:
    TokenType myType;
    QString value;
    int lineNumber;
    int linePos;
};

#endif // TOKEN_H
