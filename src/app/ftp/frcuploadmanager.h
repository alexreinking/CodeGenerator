#ifndef FRCUPLOADMANAGER_H
#define FRCUPLOADMANAGER_H

#include <QObject>
#include <QDir>
#include "synchronousftpput.h"

class FRCUploadManager : public QObject
{
    Q_OBJECT
public:
    explicit FRCUploadManager(QDir base, int teamNumber, QObject *parent = 0);
    bool deploy();

private:
    QDir baseDir;
    SyncFtp* myUploader;
};

#endif // FRCUPLOADMANAGER_H
