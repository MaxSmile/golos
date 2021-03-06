set(CURRENT_TARGET account_by_key)

list(APPEND CURRENT_TARGET_HEADERS
        include/steemit/account_by_key/account_by_key_plugin.hpp
        include/steemit/account_by_key/account_by_key_api.hpp
        include/steemit/account_by_key/account_by_key_objects.hpp
        )

list(APPEND CURRENT_TARGET_SOURCES
        account_by_key_plugin.cpp
        account_by_key_api.cpp
        )

if(BUILD_SHARED_LIBRARIES)
    add_library(golos_${CURRENT_TARGET} SHARED
            ${CURRENT_TARGET_HEADERS}
            ${CURRENT_TARGET_SOURCES}
            )
else()
    add_library(golos_${CURRENT_TARGET} STATIC
            ${CURRENT_TARGET_HEADERS}
            ${CURRENT_TARGET_SOURCES}
            )
endif()

add_library(golos::${CURRENT_TARGET} ALIAS golos_${CURRENT_TARGET})
set_property(TARGET golos_${CURRENT_TARGET} PROPERTY EXPORT_NAME ${CURRENT_TARGET})

target_link_libraries(golos_${CURRENT_TARGET} golos::chain golos::protocol golos::application)
target_include_directories(golos_${CURRENT_TARGET}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

install(TARGETS
        golos_${CURRENT_TARGET}

        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )