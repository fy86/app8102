#-------------------------------------------------
#
# Project created by QtCreator 2019-07-12T13:38:06
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = app8102
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    vwmain.cpp \
    cansendthread.cpp \
    myst.cpp \
    canrecvthread.cpp \
    parser7.cpp \
    pkt60130.cpp \
    parserfile.cpp \
    parser0.cpp \
    parserfile46.cpp \
    parserdt.cpp \
    parserimg.cpp

HEADERS += \
    vwmain.h \
    cansendthread.h \
    myst.h \
    canrecvthread.h \
    parser7.h \
    pkt60130.h \
    parserfile.h \
    parser0.h \
    parserfile46.h \
    parserdt.h \
    parserimg.h
