#include <QtCore>
#include <QtDebug>
#include <QtGui>
#include <exception>
#include <cstdlib>
#include <unistd.h>
#include <util.h>
#include "generator/frccodegenerator.h"
#include "BuildSystem/buildmanager.h"
#include "ftp/frcuploadmanager.h"
#include "gui/generatordialog.h"

int main(int argc, char *argv[])
{
    qInstallMsgHandler(coloringMessageHandler);
    if(argc == 1) { //If you just run it, it will create a GUI. This seems to be a better choice for those who aren't familiar with the command line.
        QApplication app(argc,argv);
        GeneratorDialog gd;
        gd.show();
        return app.exec();
    }
    QCoreApplication app(argc, argv); //It's actually used by QDir later in the program. The IDE can't detect that.
    QString targetLanguage = "CPP";   //For future retargeting. I will only generate for C++.
    QString inputFile = "";           //Self-explanatory.
    bool deploy  = false;             //Automatically deploy?
    bool doCompile = false;           //Automatically compile?
    int teamNumber = -1;

    int c;
    while((c = getopt(argc,argv,"cdhl:n:")) != -1)
    {
        switch(c) {
        case 'c':
            doCompile = true;
            break;
        case 'd':
            deploy = true;
            break;
        case 'l':
            targetLanguage = QString(optarg);
            break;
        case 'n':
            teamNumber = QString(optarg).toInt();
            break;
        case 'h':
            printUsage();
            return 0;
            break;
        case '?':
            dieAndPrint();
            break;
        default:
            dieAndPrint("You confused getopt()! How could you!?");
            break;
        }
    }

    if(deploy && !doCompile) {
        qWarning() << "Deploy option set without compile option. Setting compile option...";
        doCompile = true;
    }

    if(deploy &&  teamNumber == -1) {
        dieAndPrint("Deploy option set with no team number specified.");
    }

    if(teamNumber == 0) {
        dieAndPrint("Invalid team number entered!");
    }

    for(int i = optind; i < argc; i++) {
        qDebug() << "Input File:" << argv[i];
        QString f(argv[i]);
        if(QFile::exists(f))
            inputFile = f;
        else
            qFatal("File \'%s\' does not exist.",qPrintable(f));
    }

    if(inputFile.size() == 0)
        dieAndPrint("Please provide an input file name.");

    qDebug() << "Target language:" << targetLanguage.toLower();
    qDebug() << "Working in:" << QDir(".").absolutePath();
    qDebug() << "Running....";
    qDebug() << "-----------";
    qDebug() << "Step 1 : Generate code.";
    FRCCodeGenerator* codeGen = new FRCCodeGenerator(targetLanguage.toLower());
    codeGen->setInputFile(inputFile);
    codeGen->generate(QDir("."));
    if(doCompile) {
        qDebug() << "Step 2 : Compile code.";
        BuildManager* bldmgr = new BuildManager(QDir("."));
        try {
            bldmgr->build();
        } catch (BuildException& e) {
            qFatal("%s",e.what());
        }
        qDebug() << "Build completed without errors!";
    }
    if(deploy) {
        qDebug() << "Step 3 : Deploy code.";
        FRCUploadManager* upload = new FRCUploadManager(QDir("."),teamNumber);
        if(!upload->deploy())
            qFatal("Could not upload FRC_UserProgram.out");
        qDebug() << "Deploy completed without errors.";
    }
    return 0;
}
