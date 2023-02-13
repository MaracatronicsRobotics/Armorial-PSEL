TEMPLATE = app
DESTDIR  = ../bin
TARGET   = Armorial-PSEL
VERSION  = 1.0.0

# Temporary dirs
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/moc
RCC_DIR = tmp/rc

CONFIG += c++17 console
CONFIG -= app_bundle
QT += core network

DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lQt5Core -lprotobuf -lfmt

system(echo "Generating simulation proto headers" && cd include/proto/simulation && protoc --cpp_out=../ *.proto && cd ../../..)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_NAME=\\\"$$TARGET\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += PROJECT_PATH=\\\"$${PWD}\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        include/proto/command.pb.cc \
        include/proto/common.pb.cc \
        include/proto/packet.pb.cc \
        include/proto/replacement.pb.cc \
        main.cpp \
        src/entities/actuator/actuator.cpp \
        src/entities/coach/coach.cpp \
        src/entities/player/player.cpp \
        src/entities/vision/vision.cpp \
        src/entities/worldmap/worldmap.cpp \
        src/utils/types/robotcontrolpacket/robotcontrolpacket.cpp \
        src/utils/types/robotdetectionpacket/robotdetectionpacket.cpp \
        src/utils/utils.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/proto/command.pb.h \
    include/proto/common.pb.h \
    include/proto/packet.pb.h \
    include/proto/replacement.pb.h \
    src/entities/actuator/actuator.h \
    src/entities/coach/coach.h \
    src/entities/player/player.h \
    src/entities/vision/vision.h \
    src/entities/worldmap/worldmap.h \
    src/utils/types/robotcontrolpacket/robotcontrolpacket.h \
    src/utils/types/robotdetectionpacket/robotdetectionpacket.h \
    src/utils/utils.h
