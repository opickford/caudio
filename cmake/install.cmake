include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export targets
install(TARGETS caudio 
    EXPORT caudioTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Copy public headers.
install(DIRECTORY ${CAUDIO_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# version.h lives in the build dir, so must be installed separately to the other headers.
install(FILES ${CAUDIO_BINARY_DIR}/include/caudio/version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/caudio
)

install(EXPORT caudioTargets
    FILE caudioTargets.cmake
    NAMESPACE caudio::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/caudio
)

# Generate caudioConfig for find_package.
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/caudioConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/caudioConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/caudio
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/caudioConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/caudioConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/caudioConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/caudio
)