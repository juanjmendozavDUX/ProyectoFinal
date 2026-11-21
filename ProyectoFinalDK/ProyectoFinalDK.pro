QT += widgets multimedia multimediawidgets
CONFIG += c++17
TEMPLATE = app
TARGET = ProyectoFinalDK

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamewidget.cpp \
    entidades.cpp \
    personaje.cpp \
    donky.cpp \
    nivel.cpp \
    donygrafico.cpp \
    barco.cpp

HEADERS += \
    mainwindow.h \
    gamewidget.h \
    entidades.h \
    personaje.h \
    donky.h \
    nivel.h \
    donygrafico.h \
    barco.h

RESOURCES += recursos.qrc

win32:CONFIG += windows