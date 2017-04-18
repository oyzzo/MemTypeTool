#-------------------------------------------------
#
# Project created by QtCreator 2017-02-16T19:39:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MemTypeManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    credential.cpp \
    credentialwidget.cpp \
    credentialeditwindow.cpp \
    noekeon.c \
    memtype.c \
    device.cpp

HEADERS  += mainwindow.h \
    credentialwidget.h \
    credentialeditwindow.h \
    credential.h \
    noekeon_api.h \
    memtype_api.h \
    device.h

FORMS    += mainwindow.ui \
    credentialwidget.ui \
    credentialeditwindow.ui

RESOURCES += \
    resources.qrc

INCLUDEPATH += /usr/include/libusb-1.0
LIBS += -lusb-1.0
