#include <QTextStream>
#include <QDebug>
#include "lexer.h"

Lexer::Lexer(const QString &fileName)
{
    keywords << "Robot" << "Has" << "default" << "Drives" << "Bind" << "Assert" << "Filter" << "Module"
            << "filters" << "Action" << "Activate" << "Report" << "true" << "false" << "eq" << "ne"
            << "lt" << "le" << "gt" << "ge" << "Indicate";
    operators << "[" << "]" << "{`" << "`}" << "." << "," << "(" << ")" << ":" << "+" << "-"
            << "*" << "/" << "%";

    sourceFile = new QFile(fileName);
    fileData = "";
    if((fileOpen = sourceFile->open(QIODevice::ReadOnly | QIODevice::Text))) {
        QTextStream ts(sourceFile);
        fileData = ts.readAll();
        fileData += "\n"; //quick hack.
    }
}

Lexer::~Lexer()
{
    if(fileOpen)
        sourceFile->close();
    delete sourceFile;
}

QList<Token> Lexer::scan(bool *ok)
{
    if(ok && !fileOpen) {
        qCritical() << "Could not open the source file.";
        *ok = fileOpen;
        return QList<Token>();
    }

    QList<Token> ret;
    QString buffer;
    Token tmp;
    int lineNo; int linePos; int startingPos;

    LexerState currentState = Default;

    for(int i = 0; i < fileData.length(); i++) {
        char ch = fileData.at(i).toAscii();
        switch(currentState) {
        case Default:
            if(isIdentifierCharacter(ch)) {
                startingPos = i;
                buffer = ch;
                currentState = Word;
            } else if(isNumber(ch)) {
                startingPos = i;
                buffer = ch;
                currentState = Number;
            } else if(isSymbol(ch)) {
                startingPos = i;
                buffer = ch;
                currentState = Operator; //goes to string/inforeign block
            } else if(ch == '\t') {
                startingPos = i;
                buffer = "";
                do {
                    buffer += " ";
                    if(i+1 < fileData.length()) {
                        ch = fileData.at(++i).toAscii();
                    }
                } while(ch == '\t'); i--;
                getPosInfo(&lineNo,&linePos,startingPos);
                ret << Token(lineNo,linePos,Token::Tab,QString("%1").arg(buffer.length()));
            } else if(ch == '\n') {
                getPosInfo(&lineNo,&linePos,i);
                ret << Token(lineNo,linePos,Token::Break,"\\n");
                if(fileData.length() > i+1) {
                    if(fileData.at(i+1).toAscii() != '\t') {
                        ret << Token(0,0,Token::Tab,"0"); //To ease detecting syntactic indentation.
                    }
                }
            } else if(ch == ' ') {
                //Ignore spaces.
            } else { //Illegal characters.
                qWarning("Illegal character \'%c\' encountered.",ch);
                //Do nothing. Go to next character.
            }
            break;
        case Number:
            if(isNumber(ch) || ch == '.') {
                buffer += ch;
            } else {
                i--;
                getPosInfo(&lineNo,&linePos,startingPos);
                ret << Token(lineNo,linePos,Token::Number,buffer);
                currentState = Default;
            }
            break;
        case Word:
            //This works because a letter brought us to this state. No numbers at the beginning of strings.
            if(isIdentifierCharacter(ch) || isNumber(ch) || ch == '-')
                buffer += ch;
            else {
                i--;
                getPosInfo(&lineNo,&linePos,startingPos);
                ret << Token(lineNo,linePos,(keywords.contains(buffer)) ? Token::Keyword : Token::Identifier,buffer);
                currentState = Default;
            }
            break;
        case Operator:
            if(buffer == "\"") {
                buffer = ch;
                currentState = String;
            } else if(buffer == "#") {
                currentState = InComment;
            } else {
                if(isSymbol(ch))
                    buffer += ch;
                else {
                    i--;
                    while(!operators.contains(buffer) && buffer.length()) {
                        buffer.chop(1); i--;
                    }
                    if(buffer == "{`") {
                        buffer = "";
                        currentState = InForeignBlock;
                    } else {
                        getPosInfo(&lineNo,&linePos,startingPos);
                        if(buffer.length())
                            ret << Token(lineNo,linePos,Token::Operator,buffer);
                        currentState = Default;
                    }
                }
            }
            break;
        case String:
            if(ch == '\"' && !buffer.endsWith("\\")) {
                buffer.replace("\\n","\n");
                buffer.replace("\\t","\t");
                buffer.replace("\\\"","\"");
                getPosInfo(&lineNo,&linePos,startingPos);
                ret << Token(lineNo,linePos,Token::String,buffer);
                currentState = Default;
            } else {
                buffer += ch;
            }
            break;
        case InForeignString:
            if(ch == '\"' && !buffer.endsWith("\\")) {
                currentState = InForeignBlock;
            }
            buffer += ch;
            break;
        case InForeignMultiLineComment:
            if(ch == '/' && buffer.endsWith("*")) {
                currentState = InForeignBlock;
            }
            buffer += ch;
            break;
        case InForeignOneLineComment:
            if(ch == '\n') {
                currentState = InForeignBlock;
            }
            buffer += ch;
            break;
        case InForeignBlock:
            if(ch == '`') {
                if(fileData.size() > i+1) {
                    if(fileData.at(++i) == '}') {
                        getPosInfo(&lineNo,&linePos,startingPos);
                        ret << Token(lineNo,linePos,Token::ForeignBlock,buffer.trimmed());
                        currentState = Default;
                    }
                }
            } else if(ch == '\"') {
                currentState = InForeignString;
            } else if(ch == '*' && buffer.endsWith("/")) {
                currentState = InForeignMultiLineComment;
            } else if(ch == '/' && buffer.endsWith("/")) {
                currentState = InForeignOneLineComment;
            }
            buffer += ch;
            break;
        case InComment: //Goes until the end of the line. Multi-line comments not supported.
            if(ch == '\n') {
                getPosInfo(&lineNo,&linePos,i);
                ret << Token(lineNo,linePos,Token::Break,"\\n");
                currentState = Default;
            }
            break;
        default:
            currentState = Default;
            break;
        }
    }
    if(ok)
        *ok = true;
    return arrangeBlockStructure(stripBreaks(ret));
}

bool Lexer::isIdentifierCharacter(char c)
{
    return (((c >= 'A' && c <= 'Z') || (c >= 'a' && c <='z')) || c == '_');
}

bool Lexer::isNumber(char c)
{
    return (c >= '0' && c <= '9');
}

bool Lexer::isSymbol(char c)
{
    QString syms("+-*/%(){`},.[]:\"#");
    return (syms.indexOf(QChar::fromAscii(c)) != -1);
}

void Lexer::getPosInfo(int *ln, int *lp, int pos)
{
    if(pos >= fileData.length()) {
        *ln = 0;
        *lp = 0;
        return;
    }
    *ln = 1;
    *lp = 1;
    for(int i = 0; i < pos; i++) {
        ++*lp;
        if(fileData.at(i) == '\n') {
            ++*ln;
            *lp = 1;
        }
    }
}

QList<Token> Lexer::stripBreaks(QList<Token> in)
{
    bool lastWasBreak = false;
    QList<Token> ret;
    foreach(Token t, in) {
        if(lastWasBreak && t.getType() == Token::Break)
            continue;
        if(!lastWasBreak && t.getType() == Token::Break)
            lastWasBreak = true;
        if(t.getType() != Token::Break)
            lastWasBreak = false;
        ret.append(t);
    }
    return ret;
}

QList<Token> Lexer::arrangeBlockStructure(QList<Token> tokens)
{
    int lastIndentLevel = 0;
    int indentUnmatched = 0;
    if(!tokens.isEmpty() && tokens.first().getType() == Token::Tab)
        qFatal("Malformatted source. Encountered block start at beginning of file. Please remove all leading whitespace and try again.");
    for(int i = 0; i < tokens.size(); i++) {
        if(tokens.at(i).getType() == Token::Tab) {
            int indentLevel = tokens.at(i).getValue().toInt();
            if(indentLevel == lastIndentLevel) {
                tokens.removeAt(i);
                continue;
            }
            if(indentLevel < lastIndentLevel) {
                tokens.removeAt(i);
                for(int j = lastIndentLevel - indentLevel; j > 0; j--) {
                    tokens.insert(i,Token(0,0,Token::BlockClose,"}"));
                    indentUnmatched--;
                }
            } else {
                tokens.removeAt(i);
                for(int j = indentLevel - lastIndentLevel; j > 0; j--) {
                    tokens.insert(i,Token(0,0,Token::BlockOpen,"{"));
                    indentUnmatched++;
                }
            }
            lastIndentLevel = indentLevel;
        }
    }
    if(indentUnmatched) {
        qWarning("Not all blocks closed! Assuming final block reaches EOF.");
    }
    return tokens;
}
