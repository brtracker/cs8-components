HEADERS += src/cs8project.h \
    src/cs8program.h \
    src/cs8programmodel.h \
    src/cs8variable.h \
    src/cs8variablemodel.h \
    src/cs8parameter.h \
    src/cs8parametermodel.h \
    src/cs8highlighter.h \
    src/cs8programheaderview.h \
    src/cs8programdataview.h \
    src/cs8programcodeview.h \
    src/cs8ProjectComponent.h \
    src/cs8application.h \
    src/cs8libraryalias.h \
    src/cs8libraryaliasmodel.h \
    src/formvariabledetaileditor.h \
    src/cs8typemodel.h \
    src/cs8localvariablemodel.h \
    src/cs8globalvariablemodel.h \
    #src/cs8usertypemodel.h \
    src/cs8usertype.h \
    src/cs8codevalidation.h

SOURCES += src/cs8project.cpp \
    src/cs8program.cpp \
    src/cs8programmodel.cpp \
    src/cs8variable.cpp \
    src/cs8variablemodel.cpp \
    src/cs8parameter.cpp \
    src/cs8parametermodel.cpp \
    src/cs8highlighter.cpp \
    src/cs8programheaderview.cpp \
    src/cs8programdataview.cpp \
    src/cs8programcodeview.cpp \
    src/cs8application.cpp \
    src/cs8libraryalias.cpp \
    src/cs8libraryaliasmodel.cpp  \
    src/formvariabledetaileditor.cpp \
    src/cs8typemodel.cpp \
    src/cs8localvariablemodel.cpp \
    src/cs8globalvariablemodel.cpp \
    #src/cs8usertypemodel.cpp \
    src/cs8usertype.cpp \
    src/cs8codevalidation.cpp

FORMS +=  ../lib/forms/formvariabledetaileditor.ui
INCLUDEPATH = build
TEMPLATE = lib
CONFIG += console \
    staticlib

DESTDIR = ../../lib
OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
QT += xml \
    core

win32 {
 build_pass:CONFIG(debug, debug|release) {
    TARGET = cs8ProjectComponentd
    OBJECTS_DIR = buildd
    UI_DIR = build
    MOC_DIR = buildd

 } else {
    TARGET = cs8ProjectComponent
    OBJECTS_DIR = build
    UI_DIR = build
    MOC_DIR = build

  }
}
