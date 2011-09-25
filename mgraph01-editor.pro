#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T10:40:09
#
#-------------------------------------------------

QT       += core gui

TARGET = mgraph01-editor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filters/transform.cpp \
    filters/convolution.cpp \
    filters/colorcorrect.cpp \
    filters/artistic.cpp \
    filters.cpp \
    filterwrapper.cpp \
    filters/histogram.cpp \
    regioneditor.cpp

HEADERS  += mainwindow.h \
    filters/transform.h \
    filters/rgbv.h \
    filters/convolution.h \
    filters/colorcorrect.h \
    filters/artistic.h \
    ifilter.h \
    filters.h \
    filterwrapper.h \
    filters/histogram.h \
    regioneditor.h

FORMS    += mainwindow.ui















