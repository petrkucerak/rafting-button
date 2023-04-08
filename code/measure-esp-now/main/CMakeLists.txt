idf_component_register(SRCS "measure-esp-now.c"
                    INCLUDE_DIRS "."
                    REQUIRES espnow_utils)

# If some components are kept in a third git repository
# (not the project repository or ESP-IDF repository),
# you can add a call to the git_describe function in
# a component CMakeLists file in order to automatically trigger
# re-runs of CMake when the Git revision changes.                    
git_describe(PROJECT_VERSION ${COMPONENT_DIR})