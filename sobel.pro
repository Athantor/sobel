######################################################################
# Automatically generated by qmake (2.01a) pon. gru 15 22:30:47 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXX += # -std=gnu++98
QMAKE_CXXFLAGS_DEBUG += -O0 -Wall -Wextra -g3 -ggdb3 

unix {
    HARDWARE_PLATFORM = $$system(uname -m)
    contains( HARDWARE_PLATFORM, x86_64 ) {
	QMAKE_CXXFLAGS_RELEASE += -O2 -g0 -march=nocona -mtune=core2
    } else {
	    QMAKE_CXXFLAGS_RELEASE += -O2 -g0 -march=i686 -mtune=core2
    }
}

win32 {
    QMAKE_CXXFLAGS_RELEASE += -O2 -g0 -march=i686 -mtune=nocona
}



# Input
HEADERS += SobelApp.hh SobMainWin.hh
FORMS += sobel.ui
SOURCES += SMWImgOps.cc sobel.cc SobelApp.cc SobMainWin.cc SMWExtract.cc
