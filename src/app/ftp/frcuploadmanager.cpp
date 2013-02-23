#include "frcuploadmanager.h"

FRCUploadManager::FRCUploadManager(QDir base, int teamNumber, QObject *parent) :
    QObject(parent)
{
    baseDir = base.absolutePath();
    myUploader = new SyncFtp();
    if(teamNumber > 9999 || teamNumber < 0)
        teamNumber = 0;
    QString host = QString("%1").arg(teamNumber,4,10,QChar('0'));
    host.insert(2,'.');
    host.prepend("10.");
    host.append(".2");
    qDebug("Connecting to host: %s",qPrintable(host));
    myUploader->connectToHost(host,21);
    myUploader->login("root","root");
}

bool FRCUploadManager::deploy()
{
    if(!myUploader->syncRemove("/ni-rt/system/FRC_UserProgram.out")) {
        qWarning("**You can ignore this warning if it is the first time deploying to this cRio**");
        qWarning("Could not delete old image!");
    }
    QString localFile = baseDir.absolutePath() + "/PPC603gnu/" + baseDir.dirName() + "/FRC_UserProgram.out";
    qDebug("Uploading: %s",qPrintable(localFile));
    return myUploader->syncPut(localFile,"/ni-rt/system");
}
