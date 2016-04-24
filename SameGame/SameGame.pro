#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T16:36:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SameGame
TEMPLATE = app


SOURCES += main.cpp\
        samegamewindow.cpp \
    game.cpp \
    boardview.cpp

HEADERS  += \
    boardview.hpp \
    game.hpp \
    samegamewindow.hpp

FORMS    += samegamewindow.ui

QMAKE_CXXFLAGS += -std=c++11
