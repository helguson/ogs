# Source files
SET( SOURCES
	mainwindow.cpp
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/OGSFileConverter.cpp
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/FileListDialog.cpp
)

# Moc Header files
SET( MOC_HEADERS
	mainwindow.h
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/OGSFileConverter.h
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/FileListDialog.h
)

# Header files
SET( HEADERS

)

# UI files
SET( UIS
	mainwindow.ui
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/OGSFileConverter.ui
	${CMAKE_SOURCE_DIR}/Applications/Utils/OGSFileConverter/FileList.ui
)


# Run Qts user interface compiler uic on .ui files
QT4_WRAP_UI( UI_HEADERS ${UIS} )

QT4_ADD_RESOURCES( QTRESOURCES ./Img/icons.qrc ./InfoVis/html.qrc )

# Run Qts meta object compiler moc on header files
QT4_WRAP_CPP( MOC_SOURCES ${MOC_HEADERS} )

# Include the headers which are generated by uic and moc
# and include additional header
SET(SOURCE_DIR_REL ${CMAKE_CURRENT_SOURCE_DIR}/../..)
INCLUDE_DIRECTORIES(
	${SOURCE_DIR_REL}/BaseLib
	${SOURCE_DIR_REL}/MathLib
	${SOURCE_DIR_REL}/GeoLib
	${SOURCE_DIR_REL}/FileIO
	${SOURCE_DIR_REL}/MeshLib
	${SOURCE_DIR_REL}/MeshLibGEOTOOLS
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/Base
	${CMAKE_CURRENT_BINARY_DIR}/DataView
	${CMAKE_CURRENT_BINARY_DIR}/DataView/StratView
	${CMAKE_CURRENT_BINARY_DIR}/DataView/DiagramView
	${CMAKE_CURRENT_BINARY_DIR}/VtkVis
	${CMAKE_CURRENT_BINARY_DIR}/VtkAct
	${CMAKE_CURRENT_BINARY_DIR}/Applications/Utils/OGSFileConverter
	${CMAKE_CURRENT_SOURCE_DIR}/Base
	${CMAKE_CURRENT_SOURCE_DIR}/DataView
	${CMAKE_CURRENT_SOURCE_DIR}/DataView/StratView
	${CMAKE_CURRENT_SOURCE_DIR}/DataView/DiagramView
	${CMAKE_CURRENT_SOURCE_DIR}/VtkVis
	${CMAKE_CURRENT_SOURCE_DIR}/VtkAct
)

# Put moc files in a project folder
SOURCE_GROUP("UI Files" REGULAR_EXPRESSION "\\w*\\.ui")
SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION "moc_.*")

# Create the library
ADD_EXECUTABLE( DataExplorer MACOSX_BUNDLE
	main.cpp
	${SOURCES}
	${HEADERS}
	${MOC_HEADERS}
	${MOC_SOURCES}
	${UIS}
	${QTRESOURCES}
)

TARGET_LINK_LIBRARIES( DataExplorer
	${QT_LIBRARIES}
	ApplicationsLib
	BaseLib
	GeoLib
	FileIO
	InSituLib
	MeshLib
	#MSHGEOTOOLS
	QtBase
	QtDataView
	QtStratView
	VtkVis
	VtkAct
	${Boost_LIBRARIES}
	${CATALYST_LIBRARIES}
	zlib
	shp
	InfoVis
)

IF(VTK_NETCDF_FOUND)
	TARGET_LINK_LIBRARIES( DataExplorer vtkNetCDF vtkNetCDF_cxx )
ELSE()
	TARGET_LINK_LIBRARIES( DataExplorer ${Shapelib_LIBRARIES} )
ENDIF () # Shapelib_FOUND

IF (GEOTIFF_FOUND)
	TARGET_LINK_LIBRARIES( DataExplorer ${GEOTIFF_LIBRARIES} )
ENDIF () # GEOTIFF_FOUND

ADD_DEPENDENCIES ( DataExplorer VtkVis )

IF(MSVC)
	# Set linker flags
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT /IGNORE:4099")
	TARGET_LINK_LIBRARIES( DataExplorer winmm)
ENDIF(MSVC)

### OpenSG support ###
IF (VTKOSGCONVERTER_FOUND)
	USE_OPENSG(DataExplorer)
	INCLUDE_DIRECTORIES( ${VTKOSGCONVERTER_INCLUDE_DIRS} )
	TARGET_LINK_LIBRARIES( DataExplorer ${VTKOSGCONVERTER_LIBRARIES} )
ENDIF ()

IF(VTKFBXCONVERTER_FOUND)
	TARGET_LINK_LIBRARIES(DataExplorer ${VTKFBXCONVERTER_LIBRARIES})
ENDIF()

INCLUDE(AddCatalystDependency)
ADD_CATALYST_DEPENDENCY(DataExplorer)

SET_PROPERTY(TARGET DataExplorer PROPERTY FOLDER "DataExplorer")


####################
### Installation ###
####################

IF(APPLE)
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${OGS_VERSION}")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${OGS_VERSION}")
	SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${OGS_VERSION}")
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${OGS_VERSION}")
	SET(MACOSX_BUNDLE_COPYRIGHT "2013 OpenGeoSys Community")
	#SET(MACOSX_BUNDLE_ICON_FILE "audio-input-microphone.icns")
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "org.opengeosys")
	SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

	SET(MACOSX_BUNDLE_RESOURCES "${EXECUTABLE_OUTPUT_PATH}/DataExplorer.app/Contents/Resources")
	SET(MACOSX_BUNDLE_ICON "${ICONS_DIR}/${MACOSX_BUNDLE_ICON_FILE}")
	#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${MACOSX_BUNDLE_RESOURCES})
	#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MACOSX_BUNDLE_ICON} ${MACOSX_BUNDLE_RESOURCES})

	INSTALL (TARGETS DataExplorer DESTINATION .)
	SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
	INCLUDE(InstallRequiredSystemLibraries)
	INCLUDE(DeployQt4)
	INSTALL_QT4_EXECUTABLE(DataExplorer.app)

	RETURN()
ENDIF()

INSTALL (TARGETS DataExplorer RUNTIME DESTINATION bin COMPONENT ogs_gui)

IF(MSVC)
	SET(OGS_GUI_EXE ${EXECUTABLE_OUTPUT_PATH}/Release/DataExplorer.exe)
ELSE(MSVC)
	SET(OGS_GUI_EXE ${EXECUTABLE_OUTPUT_PATH}/DataExplorer)
ENDIF(MSVC)

INCLUDE(GetPrerequisites)
IF(EXISTS ${OGS_GUI_EXE})
	IF(MSVC)
		GET_PREREQUISITES(${OGS_GUI_EXE} OGS_GUI_DEPENDENCIES 1 1 "" "")
	ELSE()
		GET_PREREQUISITES(${OGS_GUI_EXE} OGS_GUI_DEPENDENCIES 0 1 "/usr/local/lib;/;${VTK_DIR};/usr/lib64;" "")
	ENDIF()
	MESSAGE (STATUS "DataExplorer depends on:")
	FOREACH(DEPENDENCY ${OGS_GUI_DEPENDENCIES})
		IF(NOT ${DEPENDENCY} STREQUAL "not") # Some bug on Linux?
			GP_RESOLVE_ITEM ("/" "${DEPENDENCY}" ${OGS_GUI_EXE} "/usr/local/lib;/;${VTK_DIR};/usr/lib64;" DEPENDENCY_PATH)
			GET_FILENAME_COMPONENT(RESOLVED_DEPENDENCY_PATH "${DEPENDENCY_PATH}" REALPATH)
			STRING(TOLOWER ${DEPENDENCY} DEPENDENCY_LOWER)
			IF("${DEPENDENCY_LOWER}" MATCHES "tiff|blas|lapack|proj|jpeg|qt|gfortran|vtk")
				SET(DEPENDENCY_PATHS ${DEPENDENCY_PATHS} ${RESOLVED_DEPENDENCY_PATH} ${DEPENDENCY_PATH})
				MESSAGE("    ${DEPENDENCY}")
			ENDIF()
		ENDIF()
	ENDFOREACH (DEPENDENCY IN ${OGS_GUI_DEPENDENCIES})
	INSTALL (FILES ${DEPENDENCY_PATHS} DESTINATION bin COMPONENT ogs_gui)
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		INSTALL(PROGRAMS data-explorer.sh DESTINATION . COMPONENT ogs_gui)
	ENDIF()
	ADD_CUSTOM_COMMAND(TARGET DataExplorer POST_BUILD COMMAND ;) # For caching: excetuting empty command
ELSE()
	# Run CMake after DataExplorer was built to run GetPrerequisites on executable
	ADD_CUSTOM_COMMAND(TARGET DataExplorer POST_BUILD COMMAND ${CMAKE_COMMAND}
		ARGS ${CMAKE_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_BINARY_DIR} VERBATIM)
ENDIF()
