if(VTKFBXCONVERTER_FOUND)
	add_definitions(-DVTKFBXCONVERTER_FOUND)
	include_directories(${VTKFBXCONVERTER_INCLUDE_DIRS})
endif()

if(VTKOSGCONVERTER_FOUND)
	add_definitions(-DVTKOSGCONVERTER_FOUND)
	include_directories(${VTKOSGCONVERTER_INCLUDE_DIRS})
endif()

# Add subprojects
add_subdirectory(Base)
add_subdirectory(DataView/StratView)
add_subdirectory(DataView)
add_subdirectory(DataView/DiagramView)
add_subdirectory(VtkVis)
add_subdirectory(VtkAct)
add_subdirectory(InfoVis)
include(DataExplorer.cmake)
