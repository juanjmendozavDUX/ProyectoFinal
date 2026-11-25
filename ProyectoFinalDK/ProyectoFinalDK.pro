QT += core gui widgets multimedia multimediawidgets

CONFIG += c++17

SOURCES += \
    avion.cpp \
    barco.cpp \
    bomba.cpp \
    donky.cpp \
    donygrafico.cpp \
    entidades.cpp \
    gamewidget.cpp \
    liana.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel.cpp \
    nivel2widget.cpp \
    objetos.cpp \
    personaje.cpp \
    pirana.cpp \
    soldado_enemigo.cpp

HEADERS += \
    avion.h \
    barco.h \
    bomba.h \
    donky.h \
    donygrafico.h \
    entidades.h \
    gamewidget.h \
    liana.h \
    mainwindow.h \
    nivel.h \
    nivel2widget.h \
    objetos.h \
    personaje.h \
    pirana.h \
    soldado_enemigo.h

RESOURCES += \
    recursos.qrc

win32:CONFIG += windows