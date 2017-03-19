TEMPLATE = app

QT       += core gui widgets

TARGET = qsshfs

DEFINES += QT_DEPRECATED_WARNINGS

include(./QPathEdit/qpathedit.pri)

HEADERS  += mainwindow.h \
	editremotedialog.h

SOURCES += main.cpp\
		mainwindow.cpp \
	editremotedialog.cpp

FORMS    += mainwindow.ui \
	editremotedialog.ui

RESOURCES += \
	res.qrc