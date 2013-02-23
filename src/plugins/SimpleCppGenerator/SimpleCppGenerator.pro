TEMPLATE = lib
CONFIG += plugin

QT       -= gui

TARGET = $$qtLibraryTarget(simplecppgen)
DESTDIR = ../../../plugins

INCLUDEPATH += ../../app/include
INCLUDEPATH += ../../app/generator/interfaces

HEADERS += \
    simplecppgenerator.h \
    bindgenerator.h

SOURCES += \
    simplecppgenerator.cpp \
    bindgenerator.cpp

RESOURCES += \
    staticFiles.qrc
