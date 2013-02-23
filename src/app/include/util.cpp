#include "util.h"

void printUsage()
{
    qDebug() << "Usage: ";
    qDebug() << "./frcgen [OPTIONS] inputfile.frc";
    qDebug() << "OPTIONS";
    qDebug() << "-------";
    qDebug() << "-c : Compile after generating";
    qDebug() << "-d : Deploy to robot after compiling";
    qDebug() << "     This option implies -c";
    qDebug() << "-l : Takes one argument. Selects the target language.";
    qDebug() << "     C++ is the default.";
    qDebug() << "-n : Takes one argument. Sets the team number. Required for -d.";
    qDebug() << "-h : Display this help message.";
}

void dieAndPrint(const QString &message)
{
    if(message.size())
        qFatal("%s",qPrintable(message));
    printUsage();
    exit(1);
}

QString addColorToString(const QString &in, const QString &color)
{
#ifndef Q_OS_LINUX
    return in;
#endif
    QHash<QString, QString> colorHash;
    colorHash.insert("clear","\E[0m");
    colorHash.insert("white","\E[1m");
    colorHash.insert("black","\E[30m");
    colorHash.insert("grey","\E[1;30m");
    colorHash.insert("gray","\E[1;30m"); //Different spellings. Even I'm not consistent.
    colorHash.insert("red","\E[31m");
    colorHash.insert("orange","\E[1;31m");
    colorHash.insert("green","\E[32m");
    colorHash.insert("lightgreen","\E[1;32m");
    colorHash.insert("brown","\E[33m");
    colorHash.insert("yellow","\E[1;33m");
    colorHash.insert("blue","\E[34m");
    colorHash.insert("lightblue","\E[1;34m");
    colorHash.insert("indigo","\E[1;34m");
    colorHash.insert("purple","\E[35m");
    colorHash.insert("violet","\E[35m");
    colorHash.insert("lightpurple","\E[1;35m");
    colorHash.insert("teal","\E[36m");
    colorHash.insert("aqua","\E[1;36m");
    colorHash.insert("lightgrey","\E[37m");
    colorHash.insert("lightgray","\E[37m");
    colorHash.insert("offwhite","\E[1;37m");
    return colorHash.value(color,"\E[0m") + in + "\E[0m";
}

void coloringMessageHandler(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "%s\n", msg);
        break;
    case QtWarningMsg:
#ifdef Q_OS_LINUX
        fprintf(stderr, "\E[1;36mWarning:\E[0m %s\n", msg);
#else
        fprintf(stderr, "%s\n", msg);
#endif
        break;
    case QtCriticalMsg:
#ifdef Q_OS_LINUX
        fprintf(stderr, "\E[1;31mCritical:\E[0m %s\n", msg);
#else
        fprintf(stderr, "%s\n", msg);
#endif
        break;
    case QtFatalMsg:
#ifdef Q_OS_LINUX
        fprintf(stderr, "\E[1;37;41mFatal:\E[0m \E[1;31m%s\E[0m\n", msg);
        abort();
#else
        fprintf(stderr, "%s\n", msg);
        abort();
#endif
    }
}
