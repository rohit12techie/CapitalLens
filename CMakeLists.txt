cmake_minimum_required(VERSION 3.5)

# Project name
project(CapitalLens)

# Find the required Qt5 libraries
find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

# Add the source files
set(SOURCES
    src/main.cc
    src/dashboardpage.cc
    src/investmentpage.cc
    src/monthselector.cc
    src/login.cc
    src/portfolio.cc
    src/cashincashout.cc
    src/reports.cc
    src/dbmanager.cc
    src/monthselectorlayout.cc
    src/entryrow.cc
    src/entrygroupbox.cc
    src/totallabel.cc
)

# Add the header files for automatic MOC processing
set(HEADERS
    src/dashboardpage.h
    src/investmentpage.h
    src/monthselector.h
    src/login.h
    src/portfolio.h
    src/cashincashout.h
    src/reports.h
    src/dbmanager.h
    src/monthselectorlayout.h
    src/entryrow.h
    src/entrygroupbox.h
    src/totallabel.h
)

set(CMAKE_BUILD_TYPE Debug)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link the executable with Qt5 Widgets
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql)

# Copy the schema.sql file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/resources/schema.sql
               ${CMAKE_BINARY_DIR}/resources/schema.sql
               COPYONLY)

# Set the runtime working directory to include the resources folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# Include resources path for runtime
target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCES_PATH="${CMAKE_BINARY_DIR}/resources"
)

# Include directories for headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Automatically include Qt's MOC-generated files
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
target_sources(CapitalLens PRIVATE ${MOC_SOURCES})
