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


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)


include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR}) 
include_directories(${GLEW_INCLUDE_DIRS})


if(WIN32)

    message(STATUS "Configuring for Windows")
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")
    message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})

    include_directories(${TOOLKITS_FOLDER}/glut)

    # Check for GLUT files
    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found in specified TOOLKITS_FOLDER")
    endif()

    # Link libraries for Windows
    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib 
        ${GLEW_LIBRARIES}
    )

    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Better location
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else()
    message(STATUS "Configuring for Linux/Mac")

    if(NOT OPENGL_FOUND)
        message(FATAL_ERROR "OpenGL not found!")
    endif()
    if(NOT GLUT_FOUND)
       message(FATAL_ERROR "GLUT not found!")
    endif()
    if(NOT GLEW_FOUND) # <-- Add GLEW check
        message(FATAL_ERROR "GLEW not found!")
    endif()

    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}  
        ${GLUT_LIBRARIES} 
        ${GLEW_LIBRARIES} 
    )

endif()


set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True)


# --- END OF FILE CMakeLists.txt ---






