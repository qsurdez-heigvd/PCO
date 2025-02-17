#message("Building student project")
include(../QtrainSim/QtrainSim.pri)

CONFIG += c++17

LIBS += -lpcosynchro

HEADERS +=  \
    src/etape.h \
    src/gare.h \
    src/locomotive.h \
    src/launchable.h \
    src/locomotivebehavior.h \
    src/parcours.h \
    src/synchro.h \
    src/synchrointerface.h

SOURCES +=  \
    src/locomotive.cpp \
    src/cppmain.cpp \
    src/locomotivebehavior.cpp

DISTFILES  += \
    ../README.md \
    ../trainsim1.0.pdf \
    ../labo4.pdf
