#ifndef LEXER_H
#define LEXER_H

#include <QList>
#include <QFile>
#include <QStringList>
#include "../datastructures/token.h"

class Lexer
{
public:
    Lexer(const QString &fileName);
    ~Lexer();

    QList<Token> scan(bool *ok = 0); //This is a big FSM.

private:
    enum LexerState { Default, Number, String, Operator, Word, InComment,
                      InForeignBlock, InForeignString, InForeignOneLineComment,
                      InForeignMultiLineComment };

    QStringList keywords;
    QStringList operators;
    QFile *sourceFile;
    QString fileData;
    bool fileOpen;

    //Helper functions.
    inline bool isIdentifierCharacter(char c);
    inline bool isNumber(char c);
    inline bool isSymbol(char c);
    void getPosInfo(int* ln, int* lp, int pos);

    //Post processing
    QList<Token> stripBreaks(QList<Token> in);
    QList<Token> arrangeBlockStructure(QList<Token> tokens);
};

#endif // LEXER_H
