QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#编译的临时文件
MOC_DIR = tmp/moc
RCC_DIR = tmp/rcc
UI_DIR = tmp/ui
OBJECTS_DIR = tmp/obj

SOURCES += \
    hikcamera.cpp \
    httpserver.cpp \
    main.cpp \
    mainwindow.cpp \
    rtspplayer.cpp \
    videomanager.cpp \
    videowidget.cpp \
    configmanager.cpp \
    databasemanager.cpp

HEADERS += \
    hikcamera.h \
    httpserver.h \
    mainwindow.h \
    rtspplayer.h \
    videomanager.h \
    videowidget.h \
    configmanager.h \
    databasemanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.ini
