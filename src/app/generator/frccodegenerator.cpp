#include <QDebug>
#include <util.h>
#include "frccodegenerator.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semanticmodel/semanticbuilder.h"
#include "semanticmodel/semanticmodel.h"

FRCCodeGenerator::FRCCodeGenerator(const QString &targetLanguage, QDir appDirPath)
{
    if(appDirPath == QDir()) {
        this->pluginsDir = QCoreApplication::applicationDirPath();
        this->pluginsDir.cd("plugins");
    } else {
        this->pluginsDir = appDirPath;
        this->pluginsDir.cd("plugins");
        qDebug("Plugin Path: %s",qPrintable(pluginsDir.absolutePath()));
    }
    setLanguage(targetLanguage);
}

void FRCCodeGenerator::setLanguage(const QString &lang)
{
    foreach(QString filename, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
        QObject *plugin = loader.instance();
        if(plugin)
        {
            GeneratorInterface* generator = qobject_cast<GeneratorInterface*>(plugin);
            if(generator) {
                languages << generator->getLanguage();
                if(generator->getLanguage().contains(lang.toLower())) {
                    this->selectedGenerator = generator;
                }
            }
        } else {
            qWarning() << "Bad library: " << qPrintable(pluginsDir.absoluteFilePath(filename)) << endl;
        }
    }
    if(!this->selectedGenerator)
        qFatal("%s",qPrintable("No suitable plugin found for language: " + lang));
}

bool FRCCodeGenerator::setInputFile(const QString &fileName)
{
    input = fileName;
    return true;
}

bool FRCCodeGenerator::generate(const QDir &projectDir)
{
    if(selectedGenerator) {
        selectedGenerator->setProjectDirectory(projectDir);

        /*** Begin Lexing ***/
        Lexer* lex = new Lexer(input);
        bool scanWorked;
        QList<Token> tokens = lex->scan(&scanWorked);
        if(!scanWorked) {
            delete lex;
            return false;
        }
#ifdef DEBUG
        qDebug() << qPrintable(QString("Scanned %1 tokens.").arg(tokens.length()));
        foreach(Token t, tokens) {
            qDebug() << qPrintable(t.print());
        }
#endif
        /*** End Lexing ***/

        /*** Begin Parsing ***/
        Parser* parse = new Parser(tokens);
        AbstractSyntaxTree* parseResult = parse->parse();
        if(!parseResult && false) {
            delete lex;
            delete parse;
            return false;
        }
#ifdef DEBUG
        qDebug() << qPrintable(parse->printTree());
#endif
        /*** End Parsing ***/

        /*** Begin Semantic Generation ***/

        SemanticBuilder* sb = new SemanticBuilder(parseResult);
        SemanticModel* sm = sb->buildSemanticModel();
        /*** End Semantic Generation ***/

        /*** Begin Code Generation ***/
        bool genWorked = selectedGenerator->generate(sm);
        /*** End Code Generation ***/

        /*** Clean Up ***/
        delete lex;
        delete parse;
        return genWorked && scanWorked;
    }
    return false;
}
