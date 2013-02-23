#ifndef SYNCHRONOUSFTPPUT_H
#define SYNCHRONOUSFTPPUT_H

#include <QtCore>
#include <QtNetwork>

class SyncFtp : public QFtp
{
    Q_OBJECT
public:
    explicit SyncFtp(QObject *parent = 0):QFtp(parent),requestId(-1),status(false)
    {}
    virtual ~SyncFtp() {}

    bool syncPut(const QString &localFile, const QString &remoteDir);
    bool syncRemove(const QString &remoteFile);

private slots:
    void finished(int reqId, bool b);

private:
    int requestId;
    bool status;
    QEventLoop loop;
};

#endif // SYNCHRONOUSFTPPUT_H
