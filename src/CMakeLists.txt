set(APP_NAME GetBilibili-C)

aux_source_directory(. SOURCE)
aux_source_directory(cjson SOURCE)

list(APPEND SOURCE ./resources/project.rc)

add_executable(${APP_NAME} ${SOURCE})

target_link_libraries(${APP_NAME}
        PRIVATE ${CURL_LIBRARIES}
        PRIVATE ${OPENSSL_LIBRARIES}
        PRIVATE wldap32
        PRIVATE ws2_32
        PRIVATE crypt32
        )
target_include_directories(${APP_NAME}
        PRIVATE ${CURL_INCLUDE_DIRS}
        )
target_compile_definitions(${APP_NAME}
        PRIVATE -DCURL_STATICLIB
        )

install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
