# -------------------------------------------------
# Project created by QtCreator 2010-05-19T23:30:35
# -------------------------------------------------
TARGET = hardwareexpert
VERSION = 0.9.5
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    slangthread.cpp \
    exccominterface.cpp \
    planetextconsoleinterface.cpp \
    hexeditorwidget.cpp \
    tabpagerinterface.cpp \
    consolecomportinterface.cpp \
    codeeditor/codeeditor.cpp \
    consolelptportinterface.cpp \
    winioprovider.cpp \
    winiolptportinterface.cpp \
    codefile.cpp \
    consolei2cinterface.cpp \
    lptdrvprovider.cpp \
    lptdrvi2cinterface.cpp \
    intelHexProvider/intelhexprovider.cpp \
    aboutwindow.cpp \
    helpwindow.cpp \
    driverdialog.cpp \
    optionsdialog.cpp \
    slangwrapper.cpp \
    qhexedit/qhexedit_p.cpp \
    qhexedit/qhexedit.cpp \
    searchdialog.cpp \
    compairdialog.cpp \
    Timer.cpp \
    hexfilesmenager.cpp \
    motorolaSRecordProvider/motorolasrecordprovider.cpp \
    importexportmenager.cpp \
    stOsdFile/stOsdFile.cpp
HEADERS += mainwindow.h \
    slangthread.h \
    comportinterface.h \
    exccominterface.h \
    consoleinterface.h \
    planetextconsoleinterface.h \
    hexeditorwidget.h \
    memorypagerinterface.h \
    tabpagerinterface.h \
    consolecomportinterface.h \
    codeeditor/codeeditor.h \
    lptportinterface.h \
    consolelptportinterface.h \
    winioprovider.h \
    winiolptportinterface.h \
    codefile.h \
    i2cportinterface.h \
    consolei2cinterface.h \
    lptdrvprovider.h \
    lptdrvi2cinterface.h \
    intelHexProvider/intelhexprovider.h \
    aboutwindow.h \
    helpwindow.h \
    driverdialog.h \
    optionsdialog.h \
    settings.h \
    hardwareinterface.h \
    directiointerface.h \
    usbinterface.h \
    slangwrapper.h \
    qhexedit/qhexedit_p.h \
    qhexedit/qhexedit.h \
    searchdialog.h \
    compairdialog.h \
    Timer.h \
    hexfilesmenager.h \
    codeeditor/slhsyntaxhighlighter.h \
    motorolaSRecordProvider/motorolasrecordprovider.h \
    importexportmenager.h \
    stOsdFile/stOsdFile.h
FORMS += mainwindow.ui \
    aboutwindow.ui \
    helpwindow.ui \
    driverdialog.ui \
    optionsdialog.ui \
    searchdialog.ui \
    compairdialog.ui

DOCS += doc/index.t2t \
	doc/syntax.t2t \
	doc/api.t2t \
	doc/gui.t2t \
	doc/general.t2t \

LIBS += -lslang 

unix:LIBS += -lqextserialport
win32:LIBS += -lqextserialport-1.2



win32:QMAKE_LFLAGS+= -shared-libgcc 

unix:DEFINES = _TTY_POSIX_
win32:DEFINES = _TTY_WIN_
CONFIG += qt \
    thread \
    warn_on \

include( hardwareexpert-build-lib-paths.pri.user )

!include( hardwareexpert-build-lib-paths.pri.user ) {
     message( "User library path file hardwareexpert-build-lib-paths.pri.user  not found!" )
 }

RESOURCES += \
    resources.qrc

TRANSLATIONS += tr/hardwareexpert_ru.ts


doc.target = doc

doc.commands += mkdir $(OBJECTS_DIR)/doc ;
for(FILE,DOCS){
		NFILE = $$replace(FILE,t2t,html)
                doc.commands +=	txt2tags -t html --encoding=utf-8 -o  $(OBJECTS_DIR)$$NFILE $$PWD/$$FILE ;
		doc.depends += $$PWD/$$FILE
}

QMAKE_EXTRA_TARGETS += doc 
QMAKE_CLEAN += -rf doc


ver.target = ver

COMMIT = $$system(git --git-dir=$$PWD/.git rev-parse --short HEAD)

ver.commands +=	echo \"//Autogenetated file. Do not edit.\" > $(OBJECTS_DIR)/version.h ;
ver.commands +=	echo \"\" >> $(OBJECTS_DIR)/version.h ;
ver.commands +=	echo \"static const char * _current_ver =\\\"$$VERSION\\\";\" >> $(OBJECTS_DIR)/version.h ;
ver.commands +=	echo \"static const char * _current_commit =\\\"$$COMMIT\\\";\" >> $(OBJECTS_DIR)/version.h ;
ver.commands += echo \"\" >> $(OBJECTS_DIR)/version.h
doc.depends = 

QMAKE_EXTRA_TARGETS += ver
PRE_TARGETDEPS += ver
QMAKE_CLEAN += -r version.h


tr.target = tr

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

for(FILE,TRANSLATIONS){
		NFILE = $$replace(FILE,ts,qm)
		NFILE = $$replace(NFILE,tr/,)
                tr.commands +=	$$QMAKE_LRELEASE $$PWD/$$FILE -qm $(OBJECTS_DIR)$$NFILE
		tr.depends += $$PWD/$$FILE
}



QMAKE_EXTRA_TARGETS += tr
PRE_TARGETDEPS += tr
QMAKE_CLEAN += -r *.qm


deploy.target = deploy
deploy.commands += mkdir $(OBJECTS_DIR)/scripts ;
deploy.commands += cp $$PWD/tests/* $(OBJECTS_DIR)/scripts/ ;
deploy.commands += cp $$PWD/scripts/* $(OBJECTS_DIR)/scripts/ ;
deploy.commands += rm $(OBJECTS_DIR)/*.o ;
deploy.commands += rm $(OBJECTS_DIR)/*.cpp ;
deploy.commands += rm $(OBJECTS_DIR)/*.h ;
deploy.commands += rm Makefile 

QMAKE_EXTRA_TARGETS += deploy

