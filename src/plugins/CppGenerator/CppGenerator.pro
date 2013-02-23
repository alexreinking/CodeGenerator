TEMPLATE = lib
CONFIG += plugin

QT       -= gui

TARGET = $$qtLibraryTarget(cppgen)
DESTDIR = ../../../plugins

INCLUDEPATH += ../../app/include
INCLUDEPATH += ../../app/generator/interfaces

SOURCES += cppgenerator.cpp \
    drivegenerator.cpp \
    teleopgenerator.cpp \
    eventdispatchergenerator.cpp
HEADERS += cppgenerator.h \
    drivegenerator.h \
    teleopgenerator.h \
    eventdispatchergenerator.h

RESOURCES += \
    BuiltInData.qrc
