VERSION = 2.0.0
os2: {
DEFINES += 'VERSION_NUMBER=\'"2.0.0"\''
} else: {
  DEFINES += 'VERSION_NUMBER=\\\"$${VERSION}\\\"'
}


useclang{
    message ("Clang enabled")
    QMAKE_CC=clang
    QMAKE_CXX=clang
    QMAKE_CXXFLAGS += -std=c++11
}


SOURCES += eko.cpp \
    main.cpp \
    document.cpp \
    utils.cpp \
    fman.cpp \
    shortcuts.cpp \
    logmemo.cpp \
    tio.cpp \
    fx.cpp \
    3pass_eq.cpp \
    gui_utils.cpp   

HEADERS += eko.h \
    document.h \
    utils.h \
    fman.h \
    shortcuts.h \
    logmemo.h \
    tio.h \
    fx.h \
    3pass_eq.h \
    gui_utils.h 



TEMPLATE = app

CONFIG += warn_on \
    thread \
    qt \
    debug \
    link_pkgconfig

#QT += xml
QT += core
QT += gui

greaterThan(QT_MAJOR_VERSION, 4) {
       QT += widgets
   } else {
#QT += blah blah blah
   }

QMAKE_CXXFLAGS += -fpermissive

PKGCONFIG += sndfile \
             samplerate \
             portaudio-2.0


isEmpty(PREFIX):PREFIX = /usr/local #path to install
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX

TARGET = eko
target.path = $$BINDIR

INSTALLS += target
RESOURCES += eko.qrc
TRANSLATIONS += translations/eko_ru.ts \
                translations/eko_cs.ts

DISTFILES += ChangeLog \
    COPYING \
    README \
    NEWS \
    NEWS-RU \
    AUTHORS \
    TODO \
    INSTALL \
    icons/* \
    palettes/* \
    manuals/en.html \
    manuals/ru.html \
    translations/* \
    themes/Cotton/stylesheet.css \
    themes/Plum/stylesheet.css \
    themes/Smaragd/stylesheet.css \
    themes/TEA/stylesheet.css \
    themes/Turbo/stylesheet.css \
    themes/Vegan/stylesheet.css

