TEMPLATE = app
TARGET = example

QT += widgets

CONFIG += c++14

mac {
  CONFIG -= app_bundle
}

OPENCV_INSTALL = /path/to/opencv3/

INCLUDEPATH += $$PWD/.. $$OPENCV_INSTALL/include

LIBS += "-L$$OPENCV_INSTALL/lib" -lopencv_core -lopencv_imgproc

HEADERS += \
   $$PWD/../asmOpenCV.h

SOURCES += \
    $$PWD/main.cpp
