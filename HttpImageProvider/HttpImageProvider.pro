QT += network concurrent quick

CONFIG += c++17
TEMPLATE = lib
DEFINES += HTTPIMAGEPROVIDER_LIBRARY
DEFINES += QT_MESSAGELOGCONTEXT

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/AsyncImageProvider.h \
    src/httpimageprovider_global.h

SOURCES +=
