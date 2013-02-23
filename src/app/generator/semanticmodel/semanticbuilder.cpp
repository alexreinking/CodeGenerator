#include <cmath>
#include "semanticbuilder.h"

SemanticBuilder::SemanticBuilder(AbstractSyntaxTree *ast)
{
    this->ast = ast;
    symTable = new SymbolTable;
}

SemanticModel* SemanticBuilder::buildSemanticModel()
{
    SemanticModel* sm = 0;
    if(!ast)
        return 0;
    if(ast->getRobotNode()->getType() == Robot_t)
        sm = new SemanticModel(buildRobot(static_cast<RobotNode*>(ast->getRobotNode())));
    if(!sm->getRobot()->getDriveHandler())
        qFatal("No Drives statement in Robot!");
    return sm;
}

#ifdef DEBUG
void printDriveHandler(DriveHandler* dh)
{
    if(!dh)
        qFatal("No drive handler passed to printDriveHandler!");
    qDebug("DriveHandler encountered.");
    qDebug("\tMode: %s",qPrintable(dh->getMode()));
    qDebug("\tFront-left wheel: %s",qPrintable(dh->getWheel(DriveHandler::FrontLeft)->getName()));
    qDebug("\tFront-right wheel: %s",qPrintable(dh->getWheel(DriveHandler::FrontRight)->getName()));
    qDebug("\tBack-left wheel: %s",qPrintable(dh->getWheel(DriveHandler::BackLeft)->getName()));
    qDebug("\tBack-right wheel: %s",qPrintable(dh->getWheel(DriveHandler::BackRight)->getName()));
    qDebug("\tPrimary Joystick: %s",qPrintable(dh->getJoystick(DriveHandler::PrimaryJoystick)->getName()));
    if(dh->getJoystick(DriveHandler::SecondaryJoystick))
        qDebug("\tSecondary Joystick: %s",qPrintable(dh->getJoystick(DriveHandler::SecondaryJoystick)->getName()));
}
#else
#define printDriveHandler
#endif

Robot* SemanticBuilder::buildRobot(RobotNode *robot)
{
    Robot* ret = new Robot(robot->getName());
    foreach(TreeNode* tn, robot->getChildren()) {
        if(tn->getType() == Has)
            ret->addObject(buildFromHas(static_cast<HasNode*>(tn)));
        else if(tn->getType() == Drives)
            ret->setDriveHandler(buildDriveHandler(static_cast<DrivesNode*>(tn)));
        else if(tn->getType() == Indicate)
            ret->addObject(buildIndicator(static_cast<IndicateNode*>(tn)));
        else if(tn->getType() == Bind)
            ret->addObject(buildBinding(static_cast<BindNode*>(tn)));
    }
    printDriveHandler(ret->getDriveHandler());
    return ret;
}

RobotObject* SemanticBuilder::buildFromHas(HasNode *has)
{
    if(has->getTypeName().toLower() == "wheel") {
        Wheel* wh = buildWheel(has);
        symTable->insert(wh);
        return wh;
    } else if(has->getTypeName().toLower() == "joystick") {
        Joystick* joy = buildJoystick(has);
        symTable->insert(joy);
        return joy;
    } else if(has->getTypeName().toLower() == "gyro") {
        Gyro *g = buildGyro(has);
        symTable->insert(g);
        return g;
    }
    return 0;
}

#ifdef DEBUG
QString printAndReturn(const QString &val) {
    qDebug("%s",qPrintable(val));
    return val;
}
#else
#define printAndReturn
#endif

Wheel* SemanticBuilder::buildWheel(HasNode *hasWheel)
{
    //Default values:
    QString inverted = evaluateExpression(hasWheel->getOptions().value("inverted"));
    QString type = evaluateExpression(hasWheel->getOptions().value("type"));
    if(inverted == "%null") inverted = "%f";
    if(type == "%null") type = "$Jaguar";
    return new Wheel(hasWheel->getName(),
                     printAndReturn(evaluateExpression(hasWheel->getOptions().value("port"))),
                     printAndReturn(evaluateExpression(hasWheel->getOptions().value("channel"))),
                     printAndReturn(inverted),
                     printAndReturn(type),
                     hasWheel->getDefault());
}

Joystick* SemanticBuilder::buildJoystick(HasNode *hasJoystick)
{
    //Default values:
    QString jst = evaluateExpression(hasJoystick->getOptions().value("type"));
    Joystick::JoystickType jt = (jst == "$Attack3") ? Joystick::Attack3 : Joystick::Extreme3D;
    printAndReturn(((jt == Joystick::Attack3) ? "$Attack3" : "$Extreme3D"));
    QString expnt = evaluateExpression(hasJoystick->getOptions().value("exponent"));
    bool eOk = false;
    double exp = numberFromExpressionString(expnt,&eOk);
    if(!eOk)
        exp = 0.0;
    return new Joystick(printAndReturn(hasJoystick->getName()),
                        int(numberFromExpressionString(printAndReturn(evaluateExpression(hasJoystick->getOptions().value("port"))))),
                        jt,
                        hasJoystick->getDefault(),
                        exp);
}

Gyro* SemanticBuilder::buildGyro(HasNode *hasGyro)
{
    return new Gyro(hasGyro->getName(),int(numberFromExpressionString(evaluateExpression(hasGyro->getOptions().value("channel")))),hasGyro->getDefault());
}

RobotIndicator* SemanticBuilder::buildIndicator(IndicateNode *ind)
{
    RobotIndicator *ri = new RobotIndicator(ind->getCode());
    ri->setFormat(evaluateExpression(ind->getOptions().value("format")).mid(1));
    ri->setName(evaluateExpression(ind->getOptions().value("name")).mid(1));
    return ri;
}

RobotBinding* SemanticBuilder::buildBinding(BindNode *bind)
{
    return new RobotBinding(bind->getSender(),bind->getReceiver(),bind->isForeignCode());
}

DriveHandler* SemanticBuilder::buildDriveHandler(DrivesNode *drives)
{
    DriveHandler* dh = new DriveHandler(drives->getDriveType());
    if(drives->getOptions().value("primary-joystick")) {
        QString symbol = evaluateExpression(drives->getOptions().value("primary-joystick"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for primary-joystick!");
        RobotObject* ro = symTable->getObject(symbol.mid(1));
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol.mid(1)));
        Joystick* joy1 = static_cast<Joystick*>(ro);
        if(!joy1)
            qFatal("Object \'%s\' is not a Joystick!!", qPrintable(symbol.mid(1)));
        dh->setJoystick(joy1,DriveHandler::PrimaryJoystick);
    } else {
        RobotObject *ro = symTable->getDefaultValue(RobotObject::JoystickObject);
        if(!ro)
            qFatal("No default joystick specified!");
        Joystick* joy1 = static_cast<Joystick*>(ro);
        if(!joy1)
            qFatal("Default joystick is somehow not a Joystick!!");
        dh->setJoystick(joy1,DriveHandler::PrimaryJoystick);
    }
    if(drives->getOptions().value("secondary-joystick")) {
        QString symbol = evaluateExpression(drives->getOptions().value("secondary-joystick"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for secondary-joystick!");
        RobotObject* ro = symTable->getObject(symbol.mid(1));
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol.mid(1)));
        Joystick* joy2 = static_cast<Joystick*>(ro);
        if(!joy2)
            qFatal("Object \'%s\' is not a Joystick!!", qPrintable(symbol.mid(1)));
        dh->setJoystick(joy2,DriveHandler::SecondaryJoystick);
    }

    if(!drives->getOptions().value("front-left"))
        qWarning("No front left wheel specified!");
    if(!drives->getOptions().value("back-left"))
        qWarning("No back left wheel specified!");
    if(!drives->getOptions().value("front-right"))
        qWarning("No front right wheel specified!");
    if(!drives->getOptions().value("back-right"))
        qWarning("No back right wheel specified!");

    if(drives->getOptions().value("left") && drives->getOptions().value("right")) { //To support two-wheel setups
        //Front-left wheel
        QString symbol = evaluateExpression(drives->getOptions().value("left"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for left!");
        symbol = symbol.mid(1);
        RobotObject* ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        if(ro->getType() != RobotObject::WheelObject)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(static_cast<Wheel*>(ro),DriveHandler::FrontLeft);

        //Back-left wheel
        symbol = evaluateExpression(drives->getOptions().value("right"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for right!");
        symbol = symbol.mid(1);
        ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        Wheel *wheel = static_cast<Wheel*>(ro);
        if(!wheel)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(wheel,DriveHandler::FrontRight);
    } else {
        //Front-left wheel
        QString symbol = evaluateExpression(drives->getOptions().value("front-left"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for front-left!");
        symbol = symbol.mid(1);
        RobotObject* ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        if(ro->getType() != RobotObject::WheelObject)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(static_cast<Wheel*>(ro),DriveHandler::FrontLeft);

        //Back-left wheel
        symbol = evaluateExpression(drives->getOptions().value("back-left"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for back-left!");
        symbol = symbol.mid(1);
        ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        Wheel *wheel = static_cast<Wheel*>(ro);
        if(!wheel)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(wheel,DriveHandler::BackLeft);

        //Front-right wheel
        symbol = evaluateExpression(drives->getOptions().value("front-right"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for front-right!");
        symbol = symbol.mid(1);
        ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        wheel = static_cast<Wheel*>(ro);
        if(!wheel)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(wheel,DriveHandler::FrontRight);

        //Back-right wheel
        symbol = evaluateExpression(drives->getOptions().value("back-right"));
        if(!symbol.startsWith("$"))
            qFatal("DriveHandler passed a non-identifier for back-right!");
        symbol = symbol.mid(1);
        ro = symTable->getObject(symbol);
        if(!ro)
            qFatal("No object with name \'%s\' in symbol table!", qPrintable(symbol));
        wheel = static_cast<Wheel*>(ro);
        if(!wheel)
            qFatal("Object \'%s\' is not a Wheel!!", qPrintable(symbol));
        dh->setWheel(wheel,DriveHandler::BackRight);
    }
    return dh;
}

QString SemanticBuilder::evaluateExpression(ExpressionNode *expr)
{
    if(!expr)
        return "%null";
    if(expr->getType() == Terminal) {
        return static_cast<TerminalValue*>(expr)->getValue();
    } else if(expr->getType() == Binary) {
        BinaryOperation* bo = static_cast<BinaryOperation*>(expr);
        QString lhs = evaluateExpression(bo->getLeft());
        QString rhs = evaluateExpression(bo->getRight());
        bool ok;
        double l = numberFromExpressionString(lhs,&ok);
        if(!ok) return "%null";
        double r = numberFromExpressionString(rhs,&ok);
        if(!ok) return "%null";
        if(bo->getOperator() == "+")
            return QString("#%1").arg(l+r);
        else if(bo->getOperator() == "-")
            return QString("#%1").arg(l-r);
        else if(bo->getOperator() == "*")
            return QString("#%1").arg(l*r);
        else if(bo->getOperator() == "/" && r != 0.0)  //division by zero results in an error.
            return QString("#%1").arg(l/r);
        else if(bo->getOperator() == "%")
            return QString("#%1").arg(int(floor(l)) % int(floor(r)));
        else return "%null";
    } else if(expr->getType() == Unary) {
        UnaryOperation* uo = static_cast<UnaryOperation*>(expr);
        QString str = evaluateExpression(uo->getOperand());
        bool ok;
        double num = numberFromExpressionString(str,&ok);
        if(!ok) return "%null";
        if(uo->getOperator() == "-")
            return QString("#%1").arg(-1*num);
        else if(uo->getOperator() == "not")
            return QString("#%1").arg(!num);
        else return "%null";
    }
    return "%null";
}

double SemanticBuilder::numberFromExpressionString(const QString &str, bool *ok)
{
    if(str.startsWith("%")) {
        QHash<QString, double> table; //Move this crap elsewhere.
        table.insert("pi",3.14159265);
        table.insert("t",1);
        table.insert("f",0);
        if(ok) *ok = true;
        return table.value(str.mid(1),0);
    } else if(str.startsWith("#")) {
        return str.mid(1).toDouble(ok);
    }
    if(ok) *ok = false;
    return 0.0;
}
