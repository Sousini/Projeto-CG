# cmake_minimum_required(VERSION 3.5)

# # Project Name
# PROJECT(engine)

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# add_executable(${PROJECT_NAME} main.cpp)

# find_package(OpenGL REQUIRED)
# include_directories(${OpenGL_INCLUDE_DIRS})
# link_directories(${OpenGL_LIBRARY_DIRS})
# add_definitions(${OpenGL_DEFINITIONS})

# if(NOT OPENGL_FOUND)
#     message(ERROR " OPENGL not found!")
# endif(NOT OPENGL_FOUND)

# if  (WIN32)

# 	message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
# 	set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")
	
# 	if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
# 		message(ERROR ": GLUT not found")
# 	endif (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")	
	
	
# 	include_directories(${TOOLKITS_FOLDER}/glut )
# 	target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} 
# 										  ${TOOLKITS_FOLDER}/glut/glut32.lib)
	
# 	if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )
# 		file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
# 	endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )	
	
# 	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	
# else (WIN32) #Linux and Mac

# 	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} Wno-deprecated")
# 	find_package(GLUT REQUIRED)
# 	include_directories(${GLUT_INCLUDE_DIR})
# 	link_directories(${GLUT_LIBRARY_DIRS})
# 	add_definitions(${GLUT_DEFINITIONS})
	
# 	target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GL GLU glut)

# 	if(NOT GLUT_FOUND)
# 	   message(ERROR ": GLUT not found!")
# 	endif(NOT GLUT_FOUND)
	
# endif(WIN32)

# --- START OF FILE CMakeLists.txt ---

cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --- Find Required Packages ---
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED) # <-- Add this line

# --- Add Source Files ---
# Add all your .cpp files here if more than main.cpp
add_executable(${PROJECT_NAME} main.cpp)

# --- Include Directories ---
# Add include paths from found packages
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR}) # Note: GLUT uses _DIR, not _DIRS
include_directories(${GLEW_INCLUDE_DIRS}) # <-- Add this line

# --- Linker Directories and Definitions (Less common now, but keep if needed) ---
# link_directories(${OpenGL_LIBRARY_DIRS}) # Often handled by target_link_libraries now
# link_directories(${GLUT_LIBRARY_DIRS})   # Often handled by target_link_libraries now
# add_definitions(${OpenGL_DEFINITIONS})   # Use target_compile_definitions if needed
# add_definitions(${GLUT_DEFINITIONS})     # Use target_compile_definitions if needed

# --- Platform Specific Linking ---
if(WIN32)
    # --- Windows Specific Configuration ---
    message(STATUS "Configuring for Windows")
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")
    message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})

    # Include directories for Windows (GLEW headers might also be in TOOLKITS_FOLDER)
    include_directories(${TOOLKITS_FOLDER}/glut)
    # Example: If GLEW headers are also custom located:
    # include_directories(${TOOLKITS_FOLDER}/glew/include) # Adjust path as needed

    # Check for GLUT files
    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found in specified TOOLKITS_FOLDER")
    endif()

    # Example: Check for GLEW files (adjust paths/names as needed)
    # if(NOT EXISTS "${TOOLKITS_FOLDER}/glew/include/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/lib/Release/Win32/glew32.lib")
    #    message(FATAL_ERROR "GLEW not found in specified TOOLKITS_FOLDER")
    # endif()

    # Link libraries for Windows
    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}             # Standard OpenGL libs (e.g., opengl32.lib)
        ${TOOLKITS_FOLDER}/glut/glut32.lib # Specific GLUT lib path
        ${GLEW_LIBRARIES}               # GLEW library (e.g., glew32.lib - FindGLEW should set this)
        # Example: If find_package doesn't find GLEW correctly on Windows:
        # ${TOOLKITS_FOLDER}/glew/lib/Release/Win32/glew32.lib # Adjust path as needed
    )

    # Copy DLLs if they exist
    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Better location
    endif()
    # Example: Copy GLEW DLL
    # if(EXISTS "${TOOLKITS_FOLDER}/glew/bin/Release/Win32/glew32.dll") # Adjust path
    #    file(COPY ${TOOLKITS_FOLDER}/glew/bin/Release/Win32/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
    #    file(COPY ${TOOLKITS_FOLDER}/glew/bin/Release/Win32/glew32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Better location
    # endif()

    # Set startup project for Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else() # --- Linux and Mac Configuration ---
    message(STATUS "Configuring for Linux/Mac")

    # Check if packages were found (already checked by REQUIRED, but explicit messages are fine)
    if(NOT OPENGL_FOUND)
        message(FATAL_ERROR "OpenGL not found!")
    endif()
    if(NOT GLUT_FOUND)
       message(FATAL_ERROR "GLUT not found!")
    endif()
    if(NOT GLEW_FOUND) # <-- Add GLEW check
        message(FATAL_ERROR "GLEW not found!")
    endif()

    # Suppress specific warnings if needed (example)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

    # Link libraries for Linux/Mac
    # Use the variables populated by find_package.
    # No need to explicitly link GL, GLU, glut if OPENGL_LIBRARIES and GLUT_LIBRARIES are correct.
    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}  # Should include GL and GLU (-lGL -lGLU)
        ${GLUT_LIBRARIES}    # Should include GLUT (-lglut)
        ${GLEW_LIBRARIES}    # Should include GLEW (-lGLEW)
    )

endif()

# --- Optional: Set C++ Standard (Good Practice) ---
set(CMAKE_CXX_STANDARD 11) # Or 14, 17 etc.
set(CMAKE_CXX_STANDARD_REQUIRED True)


# --- END OF FILE CMakeLists.txt ---






