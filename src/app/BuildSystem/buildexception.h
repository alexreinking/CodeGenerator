#ifndef BUILDEXCEPTION_H
#define BUILDEXCEPTION_H

#include <QString>
#include <exception>
using std::exception;

class BuildException : public exception
{
public:
    BuildException(QString message);
    virtual ~BuildException() throw() {}
    virtual const char* what() const throw() {
        return message.toLatin1().constData();
    }
private:
    QString message;
};

#endif // BUILDEXCEPTION_H
