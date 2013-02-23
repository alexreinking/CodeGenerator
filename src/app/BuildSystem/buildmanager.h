#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <QtCore>
#include <util.h>
#include "buildexception.h"

class BuildManager : public QObject
{
    Q_OBJECT
public:
    explicit BuildManager(QDir base, QObject *parent = 0);
    void build();

private:
    bool removeFullDirectory(QDir targetDir);
    QString getBuildSourceFileCommand(const QFileInfo &file);
    QString getBuildPartialImageCommand();
    QString getGenerateDependencyFilesCommand();
    QString getBuildDependencyFilesCommand();
    QString getLinkToWPILibCommand();

    QDir baseDir;
};

#endif // BUILDMANAGER_H
