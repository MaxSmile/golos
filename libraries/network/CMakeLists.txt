set(CURRENT_TARGET network)

list(APPEND ${CURRENT_TARGET}_HEADERS
        include/steemit/network/config.hpp
        include/steemit/network/core_messages.hpp
        include/steemit/network/exceptions.hpp
        include/steemit/network/message.hpp
        include/steemit/network/message_oriented_connection.hpp
        include/steemit/network/node.hpp
        include/steemit/network/peer_connection.hpp
        include/steemit/network/peer_database.hpp
        include/steemit/network/stcp_socket.hpp
        )

list(APPEND ${CURRENT_TARGET}_SOURCES
        node.cpp
        stcp_socket.cpp
        core_messages.cpp
        peer_database.cpp
        peer_connection.cpp
        message_oriented_connection.cpp
        )

if(BUILD_SHARED_LIBRARIES)
    add_library(golos_${CURRENT_TARGET} SHARED
            ${${CURRENT_TARGET}_HEADERS}
            ${${CURRENT_TARGET}_SOURCES}
            )
else()
    add_library(golos_${CURRENT_TARGET} STATIC
            ${${CURRENT_TARGET}_HEADERS}
            ${${CURRENT_TARGET}_SOURCES}
            )
endif()

add_library(golos::${CURRENT_TARGET} ALIAS golos_${CURRENT_TARGET})
set_property(TARGET golos_${CURRENT_TARGET} PROPERTY EXPORT_NAME ${CURRENT_TARGET})

target_link_libraries(golos_${CURRENT_TARGET} PUBLIC fc)
target_include_directories(golos_${CURRENT_TARGET}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE "${CMAKE_SOURCE_DIR}/libraries/protocol/include"
        )

if(MSVC)
    set_source_files_properties(node.cpp PROPERTIES COMPILE_FLAGS "/bigobj")
endif(MSVC)

if(USE_PCH)
    set_target_properties(golos_${CURRENT_TARGET} PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    cotire(golos::network)
endif(USE_PCH)

install(TARGETS
        golos_${CURRENT_TARGET}

        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
install(FILES ${${CURRENT_TARGET}_HEADERS} DESTINATION "include/steemit/${CURRENT_TARGET}")

