#ifndef PARSINGHELPERINTERFACE_H
#define PARSINGHELPERINTERFACE_H

#include <QList>
#include <QStringList>
#include "../datastructures/token.h"
#include "../datastructures/frcnodes.h"

class ParsingHelperInterface
{
public:
    virtual ~ParsingHelperInterface() {}
    virtual QStringList getSupportedDirectives() const = 0;

    virtual TreeNode* parse(QList<Token> input, int &pos) = 0;
};

Q_DECLARE_INTERFACE(ParsingHelperInterface, "com.reinking.frc.ParsingHelperInterface/1.0");

#endif // PARSINGHELPERINTERFACE_H
