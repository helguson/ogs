# Source files
set( SOURCES
	WebViewDialog.cpp
)

# Moc headers
set( MOC_HEADERS
	WebViewDialog.h
)

# Header files
set( HEADERS
)

# UI files
set ( UIS
	WebViewDialog.ui
)

# Put moc files in a project folder
source_group("UI Files" REGULAR_EXPRESSION "\\w*\\.ui")
source_group("Moc Files" REGULAR_EXPRESSION "moc_.*")

# Run Qts user interface compiler uic on .ui files
qt4_wrap_ui( UI_HEADERS ${UIS} )

# Run Qts meta object compiler moc on header files
qt4_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

# Create the library
add_library( InfoVis STATIC
	${SOURCES}
	${HEADERS}
	${MOC_HEADERS}
	${MOC_SOURCES}
	${UIS}
)

target_include_directories( InfoVis PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries( InfoVis ${QT_LIBRARIES} )

set_property(TARGET QtBase PROPERTY FOLDER "DataExplorer")
