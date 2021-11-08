QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20
QMAKE_CXXFLAGS += -O2 -Wall -Werror

CONFIG += lrelease

include (include/include.pri)
include (src/src.pri)
include (forms/forms.pri)

HEADERS +=

SOURCES +=
