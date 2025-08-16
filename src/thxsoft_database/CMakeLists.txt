set(CppFileList
        DatabaseUtils.cpp
)
set(HeaderFileList
        DatabaseUtils.h
)

set(TARGET_NAME "thxsoft_database")

add_library(${TARGET_NAME}
        STATIC
        ${HeaderFileList}
        ${CppFileList})

target_link_libraries(${TARGET_NAME}
        PUBLIC
        thxsoft_common)

target_include_directories(${TARGET_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

#install(TARGETS ${TARGET_NAME}
#        EXPORT ${TARGET_NAME}Targets
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#)
#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
#        DESTINATION "include/${TARGET_NAME}"
#        FILES_MATCHING
#        PATTERN "*.h*"
#)
#install(EXPORT ${TARGET_NAME}Targets
#        FILE ${TARGET_NAME}Targets.cmake
#        NAMESPACE ${TARGET_NAME}::
#        DESTINATION ${ThxSoftLibrary}/cmake/${TARGET_NAME}
#)
#add_library(${TARGET_NAME}::${TARGET_NAME} STATIC IMPORTED)
#set_target_properties(${TARGET_NAME}::${TARGET_NAME} PROPERTIES
#        INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include")