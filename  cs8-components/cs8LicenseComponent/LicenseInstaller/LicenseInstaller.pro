TEMPLATE = app
QT = gui core network xml
CONFIG += qt warn_on console
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = ../bin
FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp
INCLUDEPATH = ../../cs8ControllerComponent/lib/src ../lib/src
LIBS = -L../../lib -lcs8ControllerComponent -lcs8LicenseComponent
DEPENDPATH = ../lib/src
