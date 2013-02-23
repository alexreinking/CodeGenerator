#ifndef SIMPLECPPGENERATOR_H
#define SIMPLECPPGENERATOR_H

#include <QtCore>
#include <QDebug>

#include <GeneratorPlugin.h>

#include "bindgenerator.h"

class SimpleCPPGenerator : public QObject, public GeneratorInterface
{
    Q_OBJECT
    Q_INTERFACES(GeneratorInterface)

public:
    void setProjectDirectory(const QDir &dir);
    QStringList getLanguage() const { return QStringList() << "simplecpp" << "simplec++"; }

    bool generate(SemanticModel *sm);

private:
    QList<BindGenerator> bindings;

    QDir projectDir;
    QString deleteObj(RobotObject* ro);
    QString constructWheel(Wheel* wh);

    QString getJoystickBindingStatement(const QString &joyName);
};

#endif // SIMPLECPPGENERATOR_H
