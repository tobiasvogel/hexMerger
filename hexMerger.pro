######################################################################
# Automatically generated by qmake (3.0) Do. Sep. 18 02:36:42 2014
######################################################################

TEMPLATE = app
TARGET = hexMerger
INCLUDEPATH += .


QT += widgets

# Input
HEADERS += hexMerger.h
SOURCES += hexMerger.cpp main.cpp

RESOURCES = hexMerger.qrc

RC_ICONS = resources/appicon.ico

ICON = resources/appicon.icns

OTHER_FILES += \
    resources/appicon.ico \
    styles/lnxStyles.css \
    styles/macStyles.css \
    styles/winStyles.css

TRANSLATIONS = lang/hexMerger_de.ts

CONFIG += static
