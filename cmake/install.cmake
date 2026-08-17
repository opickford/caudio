include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export targets
install(TARGETS sfc 
    EXPORT sfcTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Copy public headers.
install(DIRECTORY ${SFC_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# version.h lives in the build dir, so must be installed separately to the other headers.
install(FILES ${SFC_BINARY_DIR}/include/sfc/version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sfc
)

install(EXPORT sfcTargets
    FILE sfcTargets.cmake
    NAMESPACE sfc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sfc
)

# Generate sfcConfig for find_package.
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/soundforcConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/soundforcConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sfc
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sfcConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/soundforcConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/soundforcConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sfc
)