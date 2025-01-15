cmake_minimum_required(VERSION 3.5)

# Project name
project(CapitalLens)

# Find the required Qt5 libraries
find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

# Add the source files
set(SOURCES
    main.cc
    dashboardpage.cc
    investmentpage.cc
    monthselector.cc
    login.cc
    portfolio.cc
    cashincashout.cc
    reports.cc
    dbmanager.cc
)

# Add the header files for automatic MOC processing
set(HEADERS
    dashboardpage.h
    investmentpage.h
    monthselector.h
    login.h
    portfolio.h
    cashincashout.h
    reports.h
    dbmanager.h
)

# Create the executable
add_executable(CapitalLens ${SOURCES} ${HEADERS})

# Link the executable with Qt5 Widgets
target_link_libraries(CapitalLens Qt5::Widgets Qt5::Sql)

# Set C++ standard
set_target_properties(CapitalLens PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

# Automatically include Qt's MOC-generated files
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
target_sources(CapitalLens PRIVATE ${MOC_SOURCES})
