set(file_list manual.adoc
              introduction.adoc
    )

if(HAVE_ASCIIDOC_PDF)

    add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/doc/manual.pdf
        COMMAND ${HAVE_ASCIIDOC_PDF} -D ${PROJECT_BINARY_DIR}/doc ${PROJECT_SOURCE_DIR}/doc/manual.adoc
        DEPENDS ${file_list}
        )

    add_custom_target(docpdf ALL
                  DEPENDS ${PROJECT_BINARY_DIR}/doc/manual.pdf
                  )
endif()

if(HAVE_ASCIIDOC)

    add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/doc/manual.html
        COMMAND ${HAVE_ASCIIDOC} -D ${PROJECT_BINARY_DIR}/doc ${PROJECT_SOURCE_DIR}/doc/manual.adoc
        DEPENDS ${file_list}
        )


    add_custom_target(dochtml ALL
                    DEPENDS ${PROJECT_BINARY_DIR}/doc/manual.html
                    )

endif()
