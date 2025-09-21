# Config sources
set(CONFIG_SOURCES
    config_loader.cpp
)

# Create config library
add_library(config_lib STATIC ${CONFIG_SOURCES})
target_include_directories(config_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(config_lib PUBLIC tomlplusplus::tomlplusplus)
