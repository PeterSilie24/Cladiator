TEMPLATE = app

TARGET = cladiator

DESTDIR = ../../bin/

QT += core xml widgets gui printsupport #svg

CONFIG += c++11

INCLUDEPATH += ./generated-files
INCLUDEPATH += ../../source

DEPENDPATH += ../../source

Release:INCLUDEPATH += ./generated-files/release
Release:MOC_DIR += ./generated-files/release
Release:UI_DIR += ./generated-files/release
Release:RCC_DIR += ./generated-files/release
Release:OBJECTS_DIR += release

Debug:INCLUDEPATH += ./generated-files/debug
Debug:MOC_DIR += ./generated-files/debug
Debug:UI_DIR += ./generated-files/debug
Debug:RCC_DIR += ./generated-files/debug
Debug:OBJECTS_DIR += debug

QMAKE_CXXFLAGS += -O3

include(cladiator.pri)
