TEMPLATE = app

QT       += core gui widgets

TARGET = qsshfs
APPNAME = "Qt sshfs GUI"
VERSION = 1.0.0
COMPANY = Skycoder42
BUNDLE_PREFIX = de.skycoder42

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "\"APPNAME=\\\"$$APPNAME\\\"\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\\\"$$COMPANY\\\""
DEFINES += "BUNDLE_PREFIX=\\\"$$BUNDLE_PREFIX\\\""

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_ASCII_CAST_WARNINGS

include(./QPathEdit/qpathedit.pri)
include(./QtUtils/DialogMaster/dialogmaster.pri)

HEADERS  += mainwindow.h \
	editremotedialog.h \
	mountmodel.h \
	mountinfo.h

SOURCES += main.cpp\
		mainwindow.cpp \
	editremotedialog.cpp \
	mountmodel.cpp \
	mountinfo.cpp

FORMS    += mainwindow.ui \
	editremotedialog.ui

RESOURCES += \
	res.qrc

win32 {
	QMAKE_TARGET_PRODUCT = APPNAME
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}
