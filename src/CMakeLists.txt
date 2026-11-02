
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)     # Binaries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)     # Shared libraries (.so, .dylib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)     # Static libraries (.a, .lib)


add_library(tagbase tagbase.c
                    avl_tree.c
                    tagtype.c
                    virtual.c
                    utility.c
                    readwrite.c
                    events.c
            )

target_link_libraries(tagbase pthread)

#add_compile_options(-fmacro-prefix-map=/home/$ENV{USER}/wisci/src/=)
configure_file(config.h.in config.h)

install(TARGETS tagbase DESTINATION lib)
