set(CppFileList
        SqliteBatchUpdate.cpp
)
set(HeaderFileList
        SqliteBatchUpdate.h
        msqlite/bind.hpp
        msqlite/concepts.hpp
        msqlite/db.hpp
        msqlite/empty_or_error.hpp
        msqlite/error.hpp
        msqlite/exec.hpp
        msqlite/first.hpp
        msqlite/for_each.hpp
        msqlite/onerror.hpp
        msqlite/open.hpp
        msqlite/prepare.hpp
        msqlite/reset.hpp
        msqlite/result.hpp
        msqlite/step.hpp
        msqlite/stmt.hpp
        msqlite/throws/error.hpp
        msqlite/throws/exec.hpp
        msqlite/throws/for_each.hpp
        msqlite/throws/open.hpp
        msqlite/throws/prepare.hpp
        msqlite/throws/step.hpp
        msqlite/throws/value_or_throw.hpp
        msqlite/pipes/detail/bind.hpp
        msqlite/pipes/detail/empty_or_error.hpp
        msqlite/pipes/detail/exec.hpp
        msqlite/pipes/detail/first.hpp
        msqlite/pipes/detail/for_each.hpp
        msqlite/pipes/detail/onerror.hpp
        msqlite/pipes/detail/prepare.hpp
        msqlite/pipes/detail/reset.hpp
        msqlite/pipes/detail/step.hpp
        msqlite/pipes/context.hpp
        msqlite/cache/db.hpp
        msqlite/detail/bind.hpp
        msqlite/detail/call.hpp
        msqlite/detail/empty_or_error.hpp
        msqlite/detail/exec.hpp
        msqlite/detail/for_each.hpp
        msqlite/detail/fwd_exp_exec.hpp
        msqlite/detail/onerror.hpp
        msqlite/detail/open.hpp
        msqlite/detail/prepare.hpp
        msqlite/detail/prepare_impl.hpp
        msqlite/detail/reset.hpp
        msqlite/detail/step.hpp
        tl/expected.hpp
)

set(TARGET_NAME "thxsoft_database_sqlite")

add_library(${TARGET_NAME}
        STATIC
        ${HeaderFileList}
        ${CppFileList})

find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(${TARGET_NAME}
        PUBLIC
        unofficial::sqlite3::sqlite3
        thxsoft_database)

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
#
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