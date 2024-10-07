QT += qml quick network concurrent


CONFIG += c++17
SOURCES += \
        src/main.cpp



#PWD_VIRTUALCASH = $$PWD/..
#VERSION = $$NEWVIRTUALCASHVERSION
#QMAKE_TARGET_COMPANY = "Company"
#QMAKE_TARGET_PRODUCT = VirtualCash
#QMAKE_TARGET_DESCRIPTION = ConvertTobacco
#QMAKE_TARGET_COPYRIGHT = Copyright 2023

#Enable QMessageLogContext in release build
DEFINES += QT_MESSAGELOGCONTEXT

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:RC_FILE = $$PWD/version.rc
#RC_ICONS = icon.ico

RESOURCES += \
    qml.qrc

DISTFILES += \
    qml/main.qml




win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../HttpImageProvider/release/ -lHttpImageProvider
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../HttpImageProvider/debug/ -lHttpImageProvider

INCLUDEPATH += $$PWD/../../HttpImageProvider
DEPENDPATH += $$PWD/../../HttpImageProvider
