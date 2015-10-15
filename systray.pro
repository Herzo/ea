QT += core gui network script
HEADERS       = \
    ../qt/o2/src/o1.h \
    ../qt/o2/src/o1dropbox.h \
    ../qt/o2/src/o1flickr.h \
    ../qt/o2/src/o1freshbooks.h \
    ../qt/o2/src/o1requestor.h \
    ../qt/o2/src/o1twitter.h \
    ../qt/o2/src/o2.h \
    ../qt/o2/src/o2abstractstore.h \
    ../qt/o2/src/o2facebook.h \
    ../qt/o2/src/o2gft.h \
    ../qt/o2/src/o2globals.h \
    ../qt/o2/src/o2hubic.h \
    ../qt/o2/src/o2reply.h \
    ../qt/o2/src/o2replyserver.h \
    ../qt/o2/src/o2requestor.h \
    ../qt/o2/src/o2settingsstore.h \
    ../qt/o2/src/o2skydrive.h \
    ../qt/o2/src/oxtwitter.h \
    ../qt/o2/src/simplecrypt.h \
    o1khan.h \
    optionsdialog.h \
    changeuserdialog.h \
    freedialog.h \
    listgames.h
SOURCES       = main.cpp \
    ../qt/o2/src/o1.cpp \
    ../qt/o2/src/o1requestor.cpp \
    ../qt/o2/src/o2.cpp \
    ../qt/o2/src/o2facebook.cpp \
    ../qt/o2/src/o2gft.cpp \
    ../qt/o2/src/o2hubic.cpp \
    ../qt/o2/src/o2reply.cpp \
    ../qt/o2/src/o2replyserver.cpp \
    ../qt/o2/src/o2requestor.cpp \
    ../qt/o2/src/o2settingsstore.cpp \
    ../qt/o2/src/o2skydrive.cpp \
    ../qt/o2/src/oxtwitter.cpp \
    ../qt/o2/src/simplecrypt.cpp \
    o1khan.cpp \
    optionsdialog.cpp \
    changeuserdialog.cpp \
    freedialog.cpp \
    listgames.cpp
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
    listgames.ui



unix:!macx: LIBS += -lxdo
