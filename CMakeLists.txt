cmake_minimum_required(VERSION 3.10)
project(Sistema-Emprestimo-de-Salas C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find GTK+3 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Include directories
include_directories(${GTK3_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${SQLITE3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Add executable
file(GLOB PROJECT_SOURCES
        "src/*.c"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${SQLITE3_LIBRARIES})

# Copy interface.glade and style.css to build directory
configure_file(src/views/interface.glade ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(src/assets/css/style.css ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Add this line to your existing CMakeLists.txt
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

# Create data directory in build directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)