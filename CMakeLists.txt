
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

add_definitions( -Wextra -Wall -pedantic -std=c++11 -I${PROJECT_BINARY_DIR})

if( QT5 )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Core REQUIRED )

	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

	add_definitions( ${Qt5Widgets_DEFINITIONS} )
else()
	find_package( Qt4 REQUIRED )
	add_definitions( -I${Qt4_INCLUDE_DIR} )
	add_definitions( -I${QT_HEADERS_DIR} )
	INCLUDE( ${QT_USE_FILE} )
endif()

if( NOT "${DATA_INSTALL_DIR}" )
	set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share" )
endif()

if( QT5 )
	QT5_WRAP_CPP( MOC src/statusicon.h src/shaman.h src/task.h )
	QT5_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

	add_executable( shaman-qt src/main.cpp src/shaman.cpp src/statusicon.cpp src/task.cpp ${MOC} ${TRAY_RC_SRCS} )

	TARGET_LINK_LIBRARIES( shaman-qt ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} )

	file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n" )

	message( STATUS "-----------------------------------------------" )
	message( STATUS "building Qt5 tray application" )
	message( STATUS "-----------------------------------------------" )
else()
	QT4_WRAP_CPP( MOC src/statusicon.h src/shaman.h src/task.h )
	QT4_ADD_RESOURCES( TRAY_RC_SRCS icons/icon.qrc )

	add_executable( shaman-qt src/main.cpp src/shaman.cpp src/statusicon.cpp src/task.cpp ${MOC} ${TRAY_RC_SRCS} )

	if( KDE4_SUPPORT )
		find_package( KDE4 REQUIRED )
		if( KDE4_FOUND )
			message( STATUS "-----------------------------------------------" )
			message( STATUS "building KDE4 tray application" )
			message( STATUS "-----------------------------------------------" )

			file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 1\n" )

			TARGET_LINK_LIBRARIES( shaman-qt ${QT_LIBRARIES} -lkdeui -lkdecore )

		else()
			message( STATUS "-----------------------------------------------" )
			message( STATUS "building Qt4 tray application" )
			message( STATUS "-----------------------------------------------" )
			file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n" )
			TARGET_LINK_LIBRARIES( shaman-qt ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} )
		endif()
	else()
		message( STATUS "-----------------------------------------------" )
		message( STATUS "building Qt4 tray application" )
		message( STATUS "-----------------------------------------------" )

		file( WRITE "${PROJECT_BINARY_DIR}/tray_application_type.h" "\n#define USE_KDE_STATUS_NOTIFIER 0\n" )
		TARGET_LINK_LIBRARIES( shaman-qt ${QT_LIBRARIES} )
	endif()

endif()

install( FILES icons/shaman-qt.png DESTINATION "${DATA_INSTALL_DIR}/icons" )

install( TARGETS shaman-qt RUNTIME DESTINATION bin )

install( FILES ${PROJECT_BINARY_DIR}/shaman-qt.desktop DESTINATION share/applications
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/shaman-qt.desktop"
"#!/usr/bin/env xdg-open
[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/shaman-qt
GenericName[en_US]=a Qt GUI front end to shaman
GenericName=a Qt GUI front end to shaman
Icon=shaman-qt.png
MimeType=
Name[en_US]=shaman-qt
Name=shaman-qt
NoDisplay=false
Path=
StartupNotify=true
Terminal=false
TerminalOptions=
Type=Application
X-DBUS-ServiceName=
X-DBUS-StartupType=Unique
X-KDE-SubstituteUID=false
X-KDE-Username=
Categories=Network;X-MandrivaLinux-Internet;\n")
