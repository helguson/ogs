# Source files
set(SOURCES
	MeshFromRasterDialog.cpp
	QVtkDataSetMapper.cpp
	VisPrefsDialog.cpp
	VisualizationWidget.cpp
	VtkAddFilterDialog.cpp
	VtkAlgorithmPropertyCheckbox.cpp
	VtkAlgorithmPropertyLineEdit.cpp
	VtkAlgorithmPropertyVectorEdit.cpp
	VtkAppendArrayFilter.cpp
	VtkBGImageSource.cpp
	VtkColorByHeightFilter.cpp
	VtkColorLookupTable.cpp
	VtkCompositeFilter.cpp
	VtkCompositeColorByHeightFilter.cpp
	VtkCompositeColormapToImageFilter.cpp
	VtkCompositeContourFilter.cpp
	VtkCompositeElementSelectionFilter.cpp
	VtkCompositeGeoObjectFilter.cpp
	VtkCompositeImageToCylindersFilter.cpp
	VtkCompositeLineToTubeFilter.cpp
	VtkCompositeNodeSelectionFilter.cpp
	VtkCompositePointToGlyphFilter.cpp
	VtkCompositeTextureOnSurfaceFilter.cpp
	VtkCompositeThresholdFilter.cpp
	VtkFilterFactory.cpp
	VtkGeoImageSource.cpp
	VtkImageDataToLinePolyDataFilter.cpp
	VtkMeshSource.cpp
	VtkPolylinesSource.cpp
	VtkPointsSource.cpp
	VtkRaster.cpp
	VtkStationSource.cpp
	VtkSurfacesSource.cpp
	VtkTextureOnSurfaceFilter.cpp
	VtkVisHelper.cpp
	VtkVisImageItem.cpp
	VtkVisPipeline.cpp
	VtkVisPipelineItem.cpp
	VtkVisPipelineView.cpp
	VtkVisPointSetItem.cpp
	VtkVisTabWidget.cpp
	VtkAlgorithmProperties.cpp
)

# Moc headers
set(MOC_HEADERS
	MeshFromRasterDialog.h
	QVtkDataSetMapper.h
	VisPrefsDialog.h
	VisualizationWidget.h
	VtkAddFilterDialog.h
	VtkAlgorithmProperties.h
	VtkAlgorithmPropertyLineEdit.h
	VtkAlgorithmPropertyCheckbox.h
	VtkAlgorithmPropertyVectorEdit.h
	VtkVisPipeline.h
	VtkVisPipelineView.h
	VtkVisTabWidget.h
)

# Header files
set(HEADERS
	VtkAppendArrayFilter.h
	VtkBGImageSource.h
	VtkColorByHeightFilter.h
	VtkColorLookupTable.h
	VtkCompositeFilter.h
	VtkCompositeColorByHeightFilter.h
	VtkCompositeColormapToImageFilter.h
	VtkCompositeContourFilter.h
	VtkCompositeElementSelectionFilter.h
	VtkCompositeGeoObjectFilter.h
	VtkCompositeImageToCylindersFilter.h
	VtkCompositeLineToTubeFilter.h
	VtkCompositeNodeSelectionFilter.h
	VtkCompositePointToGlyphFilter.h
	VtkCompositeTextureOnSurfaceFilter.h
	VtkCompositeThresholdFilter.h
	VtkFilterFactory.h
	VtkGeoImageSource.h
	VtkImageDataToLinePolyDataFilter.h
	VtkMeshSource.h
	VtkPolylinesSource.h
	VtkPointsSource.h
	VtkRaster.h
	VtkStationSource.h
	VtkSurfacesSource.h
	VtkTextureOnSurfaceFilter.h
	VtkVisHelper.h
	VtkVisImageItem.h
	VtkVisPipelineItem.h
	VtkVisPointSetItem.h
)

# UI files
set(UIS
	MeshFromRaster.ui
	VtkAddFilterDialogBase.ui
	VtkVisTabWidgetBase.ui
	VisualizationWidgetBase.ui
	VisPrefs.ui
)

# Put moc files in a project folder
source_group("UI Files" REGULAR_EXPRESSION "\\w*\\.ui")
source_group("Moc Files" REGULAR_EXPRESSION "moc_.*")

# Put filter in a folder
source_group("Filter Header Files" REGULAR_EXPRESSION "[.]*Filter.h")
source_group("Filter Source Files" REGULAR_EXPRESSION "[.]*Filter.cpp")

# Put vtk source files in a folder
source_group("Source Header Files" REGULAR_EXPRESSION "[.]*Source.h")
source_group("Source Source Files" REGULAR_EXPRESSION "[.]*Source.cpp")

# Run Qts user interface compiler uic on .ui files
qt4_wrap_ui(UI_HEADERS ${UIS})

# Run Qts meta object compiler moc on header files
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

set(SOURCE_DIR_REL ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(GUI_SOURCE_DIR_REL ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${SOURCE_DIR_REL}/BaseLib
	${SOURCE_DIR_REL}/GeoLib
	${SOURCE_DIR_REL}/MathLib
	${SOURCE_DIR_REL}/FileIO
	${SOURCE_DIR_REL}/MeshLib
	${GUI_SOURCE_DIR_REL}/VtkAct
	${GUI_SOURCE_DIR_REL}/Base
	${GUI_SOURCE_DIR_REL}/DataView

	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../BaseLib
	${CMAKE_CURRENT_BINARY_DIR}/../DataView
)

# Create the library
add_library(VtkVis STATIC
	${SOURCES}
	${HEADERS}
	${MOC_HEADERS}
	${MOC_SOURCES}
	${UIS}
)

include(AddCatalystDependency)
ADD_CATALYST_DEPENDENCY(VtkVis)

if(GEOTIFF_FOUND)
	include_directories(${GEOTIFF_INCLUDE_DIRS})
	target_link_libraries(VtkVis ${GEOTIFF_LIBRARIES})
endif() # GEOTIFF_FOUND

target_link_libraries(VtkVis ${QT_LIBRARIES})

add_dependencies(VtkVis QtDataView)

if(VTKOSGCONVERTER_FOUND)
	USE_OPENSG(VtkVis)
	include_directories(${VTKOSGCONVERTER_INCLUDE_DIRS})
endif()

set_property(TARGET VtkVis PROPERTY FOLDER "DataExplorer")
