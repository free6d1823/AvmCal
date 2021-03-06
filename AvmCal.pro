#-------------------------------------------------
#
# Project created by QtCreator 2018-08-09T16:33:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AvmCal
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
        mainwidget.cpp \
    floor.cpp \
    inifile/inifile.cpp \
    fecwin.cpp \
    imgview.cpp \
    fecparam.cpp \
    homowin.cpp \
    homoview.cpp \
    fectooldlg.cpp \
    imglab/Mat.cpp \
    imglab/ImgProcess.cpp \
    videosource.cpp \
    camerasource.cpp \
    CameraManager.cpp

HEADERS  += mainwidget.h \
    floor.h \
    basicobject.h \
    FecParam.h \
    inifile/inifile.h \
    fecwin.h \
    imgview.h \
    homowin.h \
    homoview.h \
    fectooldlg.h \
    imglab/Mat.h \
    imglab/ImgProcess.h \
    videosource.h \
    CameraManager.h

DISTFILES +=

RESOURCES += \
    shaders.qrc \
    textures.qrc

FORMS += \
    fecwin.ui \
    homowin.ui
