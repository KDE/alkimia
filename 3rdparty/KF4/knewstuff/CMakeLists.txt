
add_library(knewstuff STATIC
    knewstuff3/core/entryinternal.cpp
    knewstuff3/core/author.cpp
    knewstuff3/core/xmlloader.cpp
)

target_include_directories(knewstuff
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/knewstuff3
)

target_link_libraries(knewstuff
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Xml
        KF${QT_MAJOR_VERSION}::KIO
)
