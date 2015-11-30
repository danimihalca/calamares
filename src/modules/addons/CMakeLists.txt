include_directories( ${PROJECT_BINARY_DIR}/src/libcalamaresui )

get_filename_component( CURRENT_MODULE_NAME  ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

calamares_add_plugin( ${CURRENT_MODULE_NAME}
    TYPE viewmodule
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        addonsviewstep.cpp
        addonsviewpage.cpp
        addonmodel.cpp
    UI
    LINK_LIBRARIES
        calamaresui
    SHARED_LIB
)
