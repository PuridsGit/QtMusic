#-------------------------------------------------
#
# Project created by QtCreator 2015-06-18T14:54:45
#
#-------------------------------------------------

# add -DTAGLIB_STATIC somewhere

QT       += core widgets gui quickwidgets multimedia multimediawidgets sql

TARGET = MusicPlayer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    configdialog.cpp \
    scanner.cpp

HEADERS  += mainwindow.h \
    configdialog.h \
    scanner.h

FORMS    += mainwindow.ui \
    configdialog.ui



unix|win32: LIBS += -L$$PWD/../../../Libraries/taglib/lib/ -ltag

INCLUDEPATH += $$PWD/../../../Libraries/taglib/include
DEPENDPATH += $$PWD/../../../Libraries/taglib/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libraries/taglib/lib/tag.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libraries/taglib/lib/libtag.a



unix:!macx|win32: LIBS += -L$$PWD/'../../../Program Files (x86)/taglib/lib/' -ltag_c

INCLUDEPATH += $$PWD/'../../../Program Files (x86)/taglib/include'
DEPENDPATH += $$PWD/'../../../Program Files (x86)/taglib/include'

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/'../../../Program Files (x86)/taglib/lib/tag_c.lib'
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/'../../../Program Files (x86)/taglib/lib/libtag_c.a'
