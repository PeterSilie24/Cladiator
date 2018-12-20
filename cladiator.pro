TEMPLATE = app

TARGET = cladiator

win32 {
	TARGET_CUSTOM_EXT = .exe
}

DESTDIR = $${OUT_PWD}/bin

BUILD_DIR = $${OUT_PWD}/build
PRO_DIR = $${PWD}

QT += core xml widgets gui printsupport #svg

CONFIG += c++11

INCLUDEPATH += ./source

DEPENDPATH += ./source

INCLUDEPATH += $${BUILD_DIR}/generated-files
MOC_DIR = $${BUILD_DIR}/generated-files
UI_DIR = $${BUILD_DIR}/generated-files
RCC_DIR = $${BUILD_DIR}/generated-files
OBJECTS_DIR = $${BUILD_DIR}/objects

include(./cladiator.pri)

win32 {
	RC_FILE = ./source/resources/cladiator.rc
}

defineTest(copyFiles) {
	FILES = $$1
	DEST_PATH = $$2

	QMAKE_POST_LINK += $$sprintf($${QMAKE_MKDIR_CMD}, $$shell_quote($$shell_path($${DEST_PATH}))) $$escape_expand(\n\t)

	for(FILE, FILES) {
		SRC_FILE = $$shell_quote($$shell_path($${FILE}))
		BASE_FILE = $$basename(FILE)
		DEST_FILE = $$shell_quote($$shell_path($${DEST_PATH}/$${BASE_FILE}))
		
		QMAKE_POST_LINK += $(COPY) $${SRC_FILE} $${DEST_FILE} $$escape_expand(\n\t)
	}

	export(QMAKE_POST_LINK)
}

copyFiles($${PRO_DIR}/COPYING, $${DESTDIR}))
copyFiles($$files($${PRO_DIR}/icons/*), $${DESTDIR}/icons))
copyFiles($$files($${PRO_DIR}/translations/*.qm), $${DESTDIR}/translations))

win32 {
    QMAKE_POST_LINK += $$shell_quote($$shell_path($$[QT_INSTALL_BINS]/windeployqt.exe)) $$shell_quote($$shell_path($${DESTDIR}/$${TARGET}$${TARGET_CUSTOM_EXT})) $$escape_expand(\n\t)
}

unix {
	CLADIATOR_DIR = /usr/local/bin/cladiator

	cladiator.path = $${CLADIATOR_DIR}
	cladiator.files = $${DESTDIR}/cladiator
	cladiator.CONFIG = no_check_exist
	
	copying.path = $${CLADIATOR_DIR}
	copying.files = $${PRO_DIR}/COPYING

	icons.path = $${CLADIATOR_DIR}/icons
	icons.files = $$files($${PRO_DIR}/icons/*)

	translations.path = $${CLADIATOR_DIR}/translations
	translations.files = $$files($${PRO_DIR}/translations/*.qm)

	config.path = $${CLADIATOR_DIR}/config
	config.commands = echo "\"[General]\nSaveLocally=false\"" > $${config.path}/settings.ini
	config.uninstall = rm -r $${config.path}/*

	EXEC_PATH = $${CLADIATOR_DIR}/cladiator
	ICON_PATH = $${CLADIATOR_DIR}/icons/cladiator.svg

	DESKTOP = "\"[Desktop Entry]\nName=Cladiator\nGenericName=Class Diagram Editor\nGenericName[de]=Klassendiagramm-Editor\nComment=Class Diagram Editor\nExec="$${EXEC_PATH}"\nIcon="$${ICON_PATH}"\nTerminal=false\nType=Application\nStartupNotify=false\nCategories=Utility;\""

	desktop.path = /usr/share/applications
	desktop.commands = echo $${DESKTOP} > $${desktop.path}/cladiator.desktop
	desktop.uninstall = $(DEL_FILE) $${desktop.path}/cladiator.desktop

	EXEC_SCRIPT = "\""$${LITERAL_HASH}"!/bin/sh\n"$${EXEC_PATH}"\""

	execscript.path = /usr/bin
	execscript.commands = echo $${EXEC_SCRIPT} > $${execscript.path}/cladiator $$escape_expand(\n\t)
	execscript.commands += chmod +x $${execscript.path}/cladiator
	execscript.uninstall = $(DEL_FILE) $${execscript.path}/cladiator
	
	INSTALLS += icons translations config copying cladiator desktop execscript
}
