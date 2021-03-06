CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

PROJECT(RuckTrack)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

INCLUDE(Qt4Hacks)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "1")
#SET(VERSION_SUFFIX "")

SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

# check for Qt5
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5WebKit REQUIRED)
FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)

IF(WIN32)
	SET(QT_QM_RCC "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource>")
	FILE(GLOB QT_QM ${MINGW_PREFIX}/share/qt5/translations/qt_*.qm)
	FOREACH(f ${QT_QM})
		STRING(REGEX MATCH ".*qt_help.*" TMP "${f}")
		IF(NOT TMP)
			STRING(REPLACE "${MINGW_PREFIX}/share/qt4/translations/" "" fa "${f}")
			SET(QT_QM_RCC "${QT_QM_RCC}<file alias=\"${fa}\">${f}</file>")
		ENDIF()
	ENDFOREACH()
	SET(QT_QM_RCC "${QT_QM_RCC}</qresource></RCC>")
	FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt_qm.qrc "${QT_QM_RCC}")
ELSE()
	EXEC_PROGRAM(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_TRANSLATIONS" OUTPUT_VARIABLE QT_TRANSLATIONS_DIR)
	IF(EXISTS "${QT_TRANSLATIONS_DIR}")
		MESSAGE("-- Found Qt translations in ${QT_TRANSLATIONS_DIR}")
		ADD_DEFINITIONS(-D'QT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}"')
	ENDIF(EXISTS "${QT_TRANSLATIONS_DIR}")
ENDIF()

# check for GDAL
IF(NOT NO_GDAL_SUPPORT)
	FIND_PACKAGE(GDAL REQUIRED)
	SET(HAVE_GDAL_SUPPORT 1)
	ADD_DEFINITIONS(-DHAVE_GDAL_SUPPORT)
	SET(GDAL_SOURCES
			src/SrtmLayer.cpp
			src/SrtmTiff.cpp
			src/QtBzip2File.cpp
			src/3rdparty/bzip2/bzlib.c
			src/3rdparty/bzip2/crctable.c
			src/3rdparty/bzip2/compress.c
			src/3rdparty/bzip2/huffman.c
			src/3rdparty/bzip2/randtable.c
			src/3rdparty/bzip2/decompress.c
			src/3rdparty/bzip2/blocksort.c
			src/3rdparty/quazip/quazip/zip.c
			src/3rdparty/quazip/quazip/unzip.c
			src/3rdparty/quazip/quazip/quazipnewinfo.cpp
			src/3rdparty/quazip/quazip/quazipfile.cpp
			src/3rdparty/quazip/quazip/quazip.cpp
			src/3rdparty/quazip/quazip/ioapi.c)
	SET(GDAL_HEADERS
			src/SrtmLayer.h
			src/QtBzip2File.h
			src/3rdparty/quazip/quazip/quazipfile.h)
	SET(GDAL_INCLUDE_DIR ${GDAL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src/3rdparty/quazip/quazip ${CMAKE_SOURCE_DIR}/src/3rdparty/bzip2)
ELSE(NOT NO_GDAL_SUPPORT)
	SET(GDAL_INCLUDE_DIR "")
	SET(GDAL_LIBRARY "")
	SET(GDAL_SOURCES "")
	SET(GDAL_HEADERS "")
ENDIF(NOT NO_GDAL_SUPPORT)

# if Doxygen is present, add a custom target to build the documentation
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND STREQUAL "YES")
	ADD_CUSTOM_TARGET(doxygen 
		DEPENDS ${rucktrack_SOURCES} ${rucktrack_INCLUDES}
		COMMAND cd ${CMAKE_SOURCE_DIR}/src \; echo -e \"PROJECT_NUMBER=${VERSION}\" | cat Doxyfile - | ${DOXYGEN_EXECUTABLE} - 
		)
ENDIF()

# check for Qwt; if not present, take from 3rdparty directory
FIND_PACKAGE(Qwt REQUIRED)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/rucktrack.rc.in ${CMAKE_BINARY_DIR}/rucktrack.rc)

# set compiler flags
SET(CMAKE_C_FLAGS "-g -Wall ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-g -fno-exceptions -Wall ${CMAKE_CXX_FLAGS}")

SET(rucktrack_SOURCES src/main.cpp
			src/RTMainWindow.cpp
			src/PreferencesDialog.cpp
			src/GoogleMapsProvider.cpp
			src/OpenStreetMapProvider.cpp
			src/WmsMapProvider.cpp
			src/GpxFile.cpp
			src/MapProvider.cpp
			src/MapView.cpp
			src/PlotView.cpp
			src/RouteTableModel.cpp
			src/RuckTrackNetworkAccessManager.cpp
			src/StatisticsTable.cpp
			src/Segmentiser.cpp
			src/SegmentiserThread.cpp
			${GDAL_SOURCES})

SET(rucktrack_INCLUDES src/RTMainWindow.h
			src/AboutDialog.h
			src/PreferencesDialog.h
			src/GoogleMapsProvider.h
			src/OpenStreetMapProvider.h
			src/WmsMapProvider.h
			src/GpxFile.h
			src/MapProvider.h
			src/MapView.h
			src/PlotView.h
			src/RuckTrackNetworkAccessManager.h
			src/RouteTableModel.h
			src/TrackPoint.h
			src/StatisticsTable.h
			src/Segmentiser.h
			src/SegmentiserThread.h
			${GDAL_HEADERS})

IF(WIN32)
	INSTALL(FILES
				${QT_BINARY_DIR}/QtCore4.dll
				${QT_BINARY_DIR}/QtGui4.dll
				${QT_BINARY_DIR}/QtXml4.dll
				${QT_BINARY_DIR}/QtNetwork4.dll
				${QT_BINARY_DIR}/QtWebKit4.dll
				${QT_BINARY_DIR}/phonon4.dll
				${QT_BINARY_DIR}/libgdal-1.dll
				${QT_BINARY_DIR}/zlib1.dll
				DESTINATION .)
	SET(WINRC "${CMAKE_BINARY_DIR}/rucktrackrc.obj")
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
			COMMAND ${WINDRES}
				-I${CMAKE_SOURCE_DIR}
				-o${CMAKE_BINARY_DIR}/rucktrackrc.obj
				-i${CMAKE_BINARY_DIR}/rucktrack.rc
			DEPENDS ${CMAKE_BINARY_DIR}/rucktrack.rc)
ENDIF(WIN32)

SET(rucktrack_UI
					forms/rtmainwindow.ui
					forms/AboutDialog.ui
					forms/PreferencesDialog.ui
					)

SET(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/rucktrack.qrc)
QT5_WRAP_CPP(rucktrack_MOC_out ${rucktrack_INCLUDES})
QT5_WRAP_UI(rucktrack_UIC_out ${rucktrack_UI})
QT5_ADD_RESOURCES(rucktrack_RCC_out ${QRC_FILE})
IF(WIN32)
	QT5_ADD_RESOURCES(ructrack_qt_qm_out ${CMAKE_CURRENT_BINARY_DIR}/qt_qm.qrc)
ENDIF()

INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR} ${Qwt_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR})
LINK_LIBRARIES(${GDAL_LIBRARY} -lqwt-qt5 -lz)
ADD_EXECUTABLE(rucktrack ${rucktrack_SOURCES} ${rucktrack_UIC_out} ${rucktrack_MOC_out} ${rucktrack_RCC_out} ${ructrack_qt_qm_out} ${WINRC} ${qm_files})


TARGET_LINK_LIBRARIES(rucktrack
        Qt5::Gui
        Qt5::Widgets
        Qt5::WebKit
        Qt5::WebKitWidgets
        Qt5::Xml
    )

IF(WIN32)
	SET_TARGET_PROPERTIES(rucktrack PROPERTIES LINK_FLAGS "${LINK_FLAGS} -mwindows")
	ADD_CUSTOM_COMMAND(TARGET rucktrack POST_BUILD COMMAND ${STRIP} ${CMAKE_BINARY_DIR}/rucktrack.exe)
	INSTALL(TARGETS rucktrack RUNTIME DESTINATION .)
ELSE(WIN32)
	INSTALL(TARGETS rucktrack RUNTIME DESTINATION bin)
ENDIF(WIN32)

#
# rules for building localizations
#
FILE(GLOB rucktrack_LOCALES ${CMAKE_SOURCE_DIR}/resources/*.ts)
SET(ts_targets "")
SET(qm_targets "")
FILE(GLOB_RECURSE rucktrack_SOURCES_for_l10n *.cpp *.h *.ui)
FOREACH(_ts_file ${rucktrack_LOCALES})
	STRING(REPLACE "${CMAKE_SOURCE_DIR}/resources/" "" _ts_target "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_file "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_target "${_ts_target}")
	ADD_CUSTOM_TARGET(${_ts_target} COMMAND ${QT_LUPDATE_EXECUTABLE} -locations none -no-obsolete ${rucktrack_SOURCES_for_l10n} -ts ${_ts_file})
	ADD_CUSTOM_TARGET(${_qm_target} COMMAND ${QT_LRELEASE_EXECUTABLE} ${_ts_file} -qm ${_qm_file})
	LIST(APPEND ts_targets "${_ts_target}")
	LIST(APPEND qm_targets "${_qm_target}")
ENDFOREACH(_ts_file ${rucktrack_LOCALES})

ADD_CUSTOM_TARGET(update-locales)
FOREACH(_item ${ts_targets})
	ADD_DEPENDENCIES(update-locales ${_item})
ENDFOREACH(_item ${ts_targets})

ADD_CUSTOM_TARGET(finalize-locales)
FOREACH(_item ${qm_targets})
	ADD_DEPENDENCIES(finalize-locales ${_item})
ENDFOREACH(_item ${qm_targets})

#
# add dist target
#
SET(TMP "rucktrack-${VERSION}")
SET(S ${CMAKE_SOURCE_DIR})
ADD_CUSTOM_TARGET(dist
						COMMAND make clean
						COMMAND rm -rf ${TMP}
						COMMAND mkdir -p ${TMP}
						COMMAND cp ${S}/AUTHORS ${S}/build_mingw32 ${S}/build_mingw64 ${S}/CMakeLists.txt ${S}/rucktrack.pro ${S}/COPYING ${S}/INSTALL ${S}/rucktrack.rc.in ${S}/README ${S}/TODO ${TMP}
						COMMAND cp -r ${S}/cmake ${S}/resources ${S}/forms ${S}/src ${TMP}
						COMMAND rm -rf `find ${TMP} -name cmake_install.cmake` `find ${TMP} -name Makefile` `find ${TMP} -type d -name CMakeFiles` ${TMP}/CMakeCache.txt
						COMMAND tar cjf rucktrack-${VERSION}.tar.bz2 ${TMP}
						COMMAND rm -rf ${TMP})


INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RuckTrack")
SET(CPACK_PACKAGE_VENDOR "RuckTrack Developers")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "RuckTrack")
IF(WIN32)
SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources\\\\rucktrack.ico")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "rucktrack.exe")
SET(CPACK_NSIS_DISPLAY_NAME "RuckTrack")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\rucktrack.sourceforge.net")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\rucktrack.sourceforge.net")
SET(CPACK_NSIS_CONTACT "tobydox@users.sourceforge.net")
SET(CPACK_PACKAGE_EXECUTABLES "rucktrack.exe;RuckTrack")
SET(CPACK_NSIS_MENU_LINKS "rucktrack.exe;RuckTrack")
SET(CPACK_NSIS_DEFINES "!include ${CMAKE_SOURCE_DIR}/cmake/nsis/FileAssociation.nsh")
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  \\\${registerExtension} \\\"$INSTDIR\\\\rucktrack.exe\\\" \\\".gpx\\\" \\\"RuckTrack GPS track\\\"
  ")
SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  \\\${unregisterExtension} \\\".gpx\\\" \\\"RuckTrack GPS track\\\"
  ")
ELSE(WIN32)
SET(CPACK_STRIP_FILES "bin/rucktrack")
SET(CPACK_PACKAGE_EXECUTABLES "rucktrack" "RuckTrack binary")
ENDIF(WIN32)

IF(WIN64)
	SET(CPACK_PACKAGE_FILE_NAME "RuckTrack-${VERSION}-win64")
	SET(CPACK_NSIS_DEFINES "${CPACK_NSIS_DEFINES}
			InstallDir \\\"\\\$PROGRAMFILES64\\\\${CPACK_PACKAGE_INSTALL_DIRECTORY}\\\"")
ENDIF(WIN64)

SET(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/resources/logo.png")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "RuckTrack")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "RuckTrack")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Tobias Doerffel, 2009-2011")

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "rucktrack-${VERSION}")
INCLUDE(CPack)

