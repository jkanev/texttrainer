######################################################################
# Automatically generated by qmake (2.01a) Thu Dec 20 17:32:37 2012
######################################################################

TEMPLATE = app
TARGET = bin/texttrainer 
DEPENDPATH += . bin i18n include resources src ui
INCLUDEPATH += /home/jacob/software/texttrainer/texttrainer/. . include

# Input
HEADERS += include/meaning.h \
           include/phrase.h \
           include/qtexttrainer.h \
           include/word.h \
           ui/texttrainer.h
FORMS += ui/texttrainer-android.ui ui/texttrainer.ui
SOURCES += src/meaning.cpp src/qtexttrainer.cpp src/word.cpp
RESOURCES += resources/texttrainer.qrc
TRANSLATIONS += i18n/texttrainer_de.ts