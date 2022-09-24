QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CFLAGS += "-static-libgcc"
QMAKE_CXXFLAGS += "-static-libstdc++"

SOURCES += \
#    consolewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
#    consolewindow.h \
    mainwindow.h \
    qcustomplot.h

FORMS += \
#    consolewindow.ui \
    mainwindow.ui

RESOURCES += \
    resources.qrc \
    themes.qrc

INCLUDEPATH += \
    "D:\Qtprojects\libsignal_analyzer"

LIBS += \
    -L"D:\Qtprojects\libsignal_analyzer\builds\Desktop_Qt_5_12_9_MinGW_32_bit-Debug\debug" \
#    -lpsm

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
