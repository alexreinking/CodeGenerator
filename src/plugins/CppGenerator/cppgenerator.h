#ifndef CPPGENERATOR_H
#define CPPGENERATOR_H

#include <QtCore>
#include <QDebug>

#include <GeneratorPlugin.h>
#include "drivegenerator.h"
#include "teleopgenerator.h"
#include "eventdispatchergenerator.h"

class CppGenerator : public QObject, public GeneratorInterface {
    Q_OBJECT
    Q_INTERFACES(GeneratorInterface)

public:
    void setProjectDirectory(const QDir &dir);
    QStringList getLanguage() const { return QStringList() << "cpp" << "c++"; }

    bool generate(SemanticModel *sm);

private:
    DriveGenerator *dg;
    TeleopGenerator *tg;
    EventDispatcherGenerator *edg;
    QDir projectDir;
    QString deleteObj(RobotObject* ro);
};

#endif // CPPGENERATOR_H
