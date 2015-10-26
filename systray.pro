unix:!macx: LIBS += -lX11 -lXtst -lXext -lXinerama -lxkbcommon
win32:RC_ICONS += emc2icon.ico
RC_FILE = myapp.rc
QT += core gui network widgets
HEADERS       = \
    changeuserdialog.h \
    freedialog.h \
    listgames.h \
    idwindowdialog.h \
    xdo.h \
    xdo_util.h \
    xdo_version.h \
    mainwindow.h
SOURCES       = main.cpp \
    changeuserdialog.cpp \
    freedialog.cpp \
    listgames.cpp \
    idwindowdialog.cpp \
    xdo.c \
    mainwindow.cpp
RESOURCES     = systray.qrc
QT           += xml svg


INCLUDEPATH += ../qt/o2/src/

# install
target.path = $$[QT_INSTALL_EXAMPLES]/desktop/systray
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS systray.pro resources images
sources.path = $$[QT_INSTALL_EXAMPLES]/desktop/systray
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

wince* {
        CONFIG(debug, release|debug) {
                addPlugins.files = $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
        }
        CONFIG(release, release|debug) {
                addPlugins.files = $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
        }
        addPlugins.path = imageformats
        DEPLOYMENT += addPlugins
}

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
symbian: warning(This example does not work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)

FORMS += \
    changeuserdialog.ui \
    freedialog.ui \
    listgames.ui \
    idwindowdialog.ui \
    mainwindow.ui

DISTFILES += \
    images/gameselectionherzo.png \
    cpld.bash \
    einstein.rc

