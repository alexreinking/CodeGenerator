#ifndef UTIL_H
#define UTIL_H

#include <QtCore>

void printUsage();
void dieAndPrint(const QString &message = "");
void coloringMessageHandler(QtMsgType type, const char *msg);
QString addColorToString(const QString &in, const QString &color);

#endif // UTIL_H
