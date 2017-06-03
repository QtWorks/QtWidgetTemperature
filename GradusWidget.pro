#-------------------------------------------------
#
# Project created by QtCreator 2017-05-27T02:03:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GradusWidget
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp \
        graduswidget.cpp

HEADERS  += graduswidget.h

FORMS    += graduswidget.ui

QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++ -openssl-linked
