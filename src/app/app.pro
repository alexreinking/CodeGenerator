TEMPLATE = app
QT       += core network gui

TARGET = ../../frcgen
CONFIG   += console
CONFIG   -= app_bundle
#DEFINES += DEBUG
INCLUDEPATH += ./include

SOURCES += main.cpp \
    BuildSystem/buildmanager.cpp \
    include/util.cpp \
    BuildSystem/buildexception.cpp \
    ftp/synchronousftpput.cpp \
    ftp/frcuploadmanager.cpp \
    generator/frccodegenerator.cpp \
    generator/datastructures/token.cpp \
    generator/lexer/lexer.cpp \
    generator/parser/parser.cpp \
    generator/semanticmodel/symboltable.cpp \
    generator/semanticmodel/semanticbuilder.cpp \
    gui/generatordialog.cpp \
    generator/datastructures/frcnodes.cpp

HEADERS += \
    include/util.h \
    BuildSystem/buildmanager.h \
    BuildSystem/buildexception.h \
    ftp/synchronousftpput.h \
    ftp/frcuploadmanager.h \
    generator/frccodegenerator.h \
    generator/interfaces/generatorinterface.h \
    generator/datastructures/token.h \
    generator/lexer/lexer.h \
    generator/parser/parser.h \
    generator/interfaces/parsinghelperinterface.h \
    generator/datastructures/ast.h \
    generator/datastructures/treenode.h \
    generator/datastructures/frcnodes.h \
    generator/datastructures/exprnodes.h \
    generator/semanticmodel/robot.h \
    generator/semanticmodel/symboltable.h \
    generator/semanticmodel/robotobject.h \
    generator/semanticmodel/wheel.h \
    generator/semanticmodel/joystick.h \
    generator/semanticmodel/drivehandler.h \
    generator/semanticmodel/semanticbuilder.h \
    generator/semanticmodel/semanticmodel.h \
    generator/semanticmodel/constantobject.h \
    include/GeneratorPlugin.h \
    gui/generatordialog.h \
    generator/semanticmodel/indicator.h \
    generator/semanticmodel/gyro.h \
    generator/semanticmodel/robotbinding.h \
    generator/semanticmodel/robotobjects.h

FORMS += \
    gui/generatordialog.ui
