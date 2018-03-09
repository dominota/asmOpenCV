TEMPLATE = app
TARGET = test

QT += widgets

CONFIG += c++14

mac {
  CONFIG -= app_bundle
}

OPENCV_INSTALL = /path/to/opencv3/

INCLUDEPATH += $$PWD/.. $$OPENCV_INSTALL/include

LIBS += "-L$$OPENCV_INSTALL/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

HEADERS += \
   $$PWD/../asmOpenCV.h

SOURCES += \
    $$PWD/main.cpp

DEFINES += TEST_INPUT_DIR=\\\"$$_PRO_FILE_PWD_/data\\\"
