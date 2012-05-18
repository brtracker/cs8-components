TEMPLATE = app
QT = gui core \
 xml
CONFIG += qt warn_on
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
FORMS = ui/dialog.ui ui/password.ui
HEADERS = src/dialogimpl.h src/passwordimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp src/passwordimpl.cpp
LIBS = -L../../lib -lcs8LicenseComponent
INCLUDEPATH = ../lib/src
RESOURCES += rc/licenseMaker.qrc ../../resources/cs8ComponentsResources.qrc
