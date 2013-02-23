#include "buildmanager.h"

BuildManager::BuildManager(QDir base, QObject *parent) : QObject(parent)
{
    baseDir = base;
}

void BuildManager::build()
{
    QDir pwd = baseDir.absolutePath();
    QDir srcDir = baseDir;
    QQueue<QString> commandList;
    QQueue<QString> nameList;

    QFile commandLog(pwd.absolutePath() + "/frcgen.log");
    if(!commandLog.open(QIODevice::Append)) {
        throw BuildException("Could not create log file.");
    }
    QTextStream fileLog(&commandLog);
    fileLog << endl << endl << "Build Started at " << QTime::currentTime().toString("hh:mm:ss")
            << " on " << QDate::currentDate().toString("dddd MMMM dd, yyyy") << endl;

    if(srcDir.cd("src")) {
        removeFullDirectory(srcDir.absolutePath() + "/PPC603gnu");
        if(!pwd.mkpath("./PPC603gnu/"+pwd.dirName()+"_partialImage")) {
            throw BuildException("Could not create partial image directory.");
        }
        if(!pwd.mkpath("./PPC603gnu/"+pwd.dirName())) {
            throw BuildException("Could not create image directory.");
        }

        QFileInfoList sourcefiles = srcDir.entryInfoList(QStringList() << "*.cpp");
        if(sourcefiles.size() == 0)
            throw BuildException("No source files");
        foreach(QFileInfo file, sourcefiles) {
            commandList.enqueue(getBuildSourceFileCommand(file));
            nameList.enqueue(file.fileName());
            fileLog << getBuildSourceFileCommand(file) << endl;
        }
        commandList.enqueue(getBuildPartialImageCommand());
        commandList.enqueue(getGenerateDependencyFilesCommand());
        commandList.enqueue(getBuildDependencyFilesCommand());
        commandList.enqueue(getLinkToWPILibCommand());
        fileLog << (getBuildPartialImageCommand()) << endl;
        fileLog << (getGenerateDependencyFilesCommand()) << endl;
        fileLog << (getBuildDependencyFilesCommand()) << endl;
        fileLog << (getLinkToWPILibCommand()) << endl;
    } else {
        throw BuildException("Could not compile because the source directory does not exist.");
    }

    QString errorString = "";
    while(!commandList.isEmpty()) {
        QString command = commandList.dequeue();

        QTemporaryFile script;
        if(!script.open()) {
            throw BuildException("Could not create temporary shell script file.");
        }
        QString scriptName = script.fileName();
        QTextStream out(&script);
        out << "#!/bin/bash" << endl;
        out << command.toAscii() << endl;
        script.close();

        if(!nameList.isEmpty())
            qDebug() << qPrintable(QString("Building file: " + (nameList.dequeue())));

        QProcess scriptProcess;
        scriptProcess.start("bash",QStringList() << scriptName);
        if(!scriptProcess.waitForStarted())
            throw BuildException("Process could not start.");
        if(!scriptProcess.waitForFinished())
            throw BuildException("Process crashed.");

        QStringList standardError = QString(scriptProcess.readAllStandardError()).split('\n');
        foreach(QString line, standardError) {
            if(line.simplified().isEmpty())
                continue;
            if(line.simplified().startsWith("Environment") || line.simplified().startsWith("fixme:"))
                continue;
            errorString += line + "\n";
        }
        if(!errorString.isEmpty())
            throw BuildException("Error compiling!\nCommand: " + command + "\nError: " + errorString);
    }
    commandLog.close();
}

bool BuildManager::removeFullDirectory(QDir targetDir)
{
    bool err = false;
    if (targetDir.exists()) {
        QFileInfoList entries = targetDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        int count = entries.size();
        for (int i = 0; ((i < count) && (!err)); i++) {
            QFileInfo entryInfo = entries[i];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
                err = removeFullDirectory(QDir(path));
            else {
                QFile file(path);
                if (!file.remove())
                    err = true;
            }
        }
        if (!targetDir.rmdir(targetDir.absolutePath()))
            err = true;
    }
    return err;
}

QString BuildManager::getBuildSourceFileCommand(const QFileInfo &file)
{
    QDir pwd = baseDir.absolutePath();
    QDir windRiverBase(getenv("WIND_BASE"));
    QString sourceBuildTemplate = "wine " + windRiverBase.absolutePath() + "/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ccppc.exe"
                                  " -g -mcpu=603 -mstrict-align -mno-implicit-fp -mlongcall -ansi"
                                  " -Wall -MD -MP -mlongcall -I" + windRiverBase.absolutePath() + "/vxworks-6.3/target/h "
                                  "-I" + windRiverBase.absolutePath() + "/vxworks-6.3/target/h/WPIlib "
                                  "-I" + windRiverBase.absolutePath() + "/vxworks-6.3/target/h/wrn/coreip "
                                  "-DCPU=PPC603 -DTOOL_FAMILY=gnu -DTOOL=gnu -D_WRS_KERNEL -o " +
                                  pwd.absolutePath() + "/PPC603gnu/" + pwd.dirName() + "_partialImage/"+ file.baseName() +".o -c "
                                  + file.absoluteFilePath();
    return sourceBuildTemplate;
}

QString BuildManager::getBuildPartialImageCommand()
{
    QDir pwd = baseDir.absolutePath();
    QDir windRiverBase(getenv("WIND_BASE"));
    QDir objectDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName()+"_partialImage";
    QString partialImageBuildTemplate = "wine " + windRiverBase.absolutePath() + "/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ccppc.exe "
                                        "-r -nostdlib -Wl,-X -o" + objectDir.absolutePath() + "/partialImage.o "
                                        + objectDir.absolutePath() + "/*.o";
    return (partialImageBuildTemplate);
}

QString BuildManager::getGenerateDependencyFilesCommand()
{
    QDir pwd = baseDir.absolutePath();
    QDir windRiverBase(getenv("WIND_BASE"));
    QDir objectDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName()+"_partialImage";
    QDir imageDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName();

    QString depBuildTemplate = "wine " + windRiverBase.absolutePath() + "/gnu/3.4.4-vxworks-6.3/x86-win32/bin/nmppc.exe "
                               + objectDir.absolutePath() + "/partialImage.o "
                               + windRiverBase.absolutePath() + "/vxworks-6.3/target/lib/WPILib.a | wine "
                               + windRiverBase.absolutePath() + "/workbench-3.0/foundation/4.1.1/x86-win32/bin/tclsh.exe "
                               + windRiverBase.absolutePath() + "/vxworks-6.3/host/resource/hutils/tcl/munch.tcl -c ppc"
                               " > " + imageDir.absolutePath() + "/ctdt.c";
    return (depBuildTemplate);
}

QString BuildManager::getBuildDependencyFilesCommand()
{
    QDir pwd = baseDir.absolutePath();
    QDir windRiverBase(getenv("WIND_BASE"));
    QDir objectDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName()+"_partialImage";
    QDir imageDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName();

    QString depBuildTemplate = "wine " + windRiverBase.absolutePath() + "/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ccppc.exe "
                               "-g -mcpu=603 -mstrict-align -mno-implicit-fp -mlongcall -fdollars-in-identifiers -Wall -I"
                               + windRiverBase.absolutePath() + "/vxworks-6.3/target/h -I" + windRiverBase.absolutePath()
                               + "/vxworks-6.3/target/h/WPIlib -I" + windRiverBase.absolutePath()
                               + "/vxworks-6.3/target/h/wrn/coreip -DCPU=PPC603 -DTOOL_FAMILY=gnu -DTOOL=gnu -D_WRS_KERNEL -o"
                               " " + imageDir.absolutePath() + "/ctdt.o -c " + imageDir.absolutePath() + "/ctdt.c";

    return (depBuildTemplate);
}

QString BuildManager::getLinkToWPILibCommand()
{
    QDir pwd = baseDir.absolutePath();
    QDir windRiverBase(getenv("WIND_BASE"));
    QDir objectDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName()+"_partialImage";
    QDir imageDir = pwd.absolutePath() + "/PPC603gnu/"+pwd.dirName();

    QString linkTemplate = "wine " + windRiverBase.absolutePath() + "/gnu/3.4.4-vxworks-6.3/x86-win32/bin/ccppc.exe "
                           "-r -nostdlib -Wl,-X -T " + windRiverBase.absolutePath()
                           + "/vxworks-6.3/target/h/tool/gnu/ldscripts/link.OUT -o \""
                           + imageDir.absolutePath() + "/FRC_UserProgram.out\" " + imageDir.absolutePath()
                           + "/ctdt.o " + objectDir.absolutePath() + "/partialImage.o " + windRiverBase.absolutePath()
                           + "/vxworks-6.3/target/lib/WPILib.a";
    return (linkTemplate);
}
