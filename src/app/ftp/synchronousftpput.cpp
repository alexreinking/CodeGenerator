#include "synchronousftpput.h"

bool SyncFtp::syncPut(const QString &localFile, const QString &remoteDir)
{
    QFile f(localFile);
    if(!f.open(QIODevice::ReadOnly))
    {
        status = false;
        return status;
    }
    connect(this,SIGNAL(commandFinished(int,bool)),this,SLOT(finished(int,bool)));
    QString remoteTarget = remoteDir + "/" + QFileInfo(f.fileName()).baseName() + "." + QFileInfo(f.fileName()).completeSuffix();
    requestId = put(&f,remoteTarget);
    loop.exec();
    f.close();
    return status;
}

bool SyncFtp::syncRemove(const QString &remoteFile)
{
    connect(this,SIGNAL(commandFinished(int,bool)),this,SLOT(finished(int,bool)));
    requestId = remove(remoteFile);
    loop.exec();
    return status;
}

void SyncFtp::finished(int reqId, bool err)
{
    if(reqId != requestId)
        return;
    status = !err;
    loop.exit();
}
