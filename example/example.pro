TEMPLATE = app
TARGET = example

QT += widgets

CONFIG += c++14

mac {
  CONFIG -= app_bundle
}

OPENCV_INSTALL = /path/to/opencv4/

INCLUDEPATH += $$PWD/.. $$OPENCV_INSTALL/include/opencv4

LIBS += "-L$$OPENCV_INSTALL/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

HEADERS += \
   $$PWD/../asmOpenCV.h

SOURCES += \
    $$PWD/main.cpp
