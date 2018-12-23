cmake_minimum_required (VERSION 3.8)

# solution
project(TiledShading)
# project
set(APP_NAME "TiledShading")

set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )

find_package(Qt5 QUIET REQUIRED COMPONENTS Quick Concurrent)
find_package(glm QUIET REQUIRED)

find_path(GPUEngine_INCLUDE_DIR NAMES geGL/Generated/ContextCalls.h)
find_path(Assimp_INCLUDE_DIR NAMES assimp/config.h)
find_path(AssimpModelLoader_INCLUDE_DIR NAMES AssimpModelLoader.h)
find_path(QtImageLoader_INCLUDE_DIR NAMES QtImageLoader.h)
find_path(ste_INCLUDE_DIR NAMES ste/DAG.h)

find_library(geCore_LIBRARY_RELEASE geCore)
find_library(geCore_LIBRARY_DEBUG geCored)
find_library(geGL_LIBRARY_RELEASE geGL)
find_library(geGL_LIBRARY_DEBUG geGLd)
find_library(geSG_LIBRARY_RELEASE geSG)
find_library(geSG_LIBRARY_DEBUG geSGd)
find_library(geUtil_LIBRARY_RELEASE geUtil)
find_library(geUtil_LIBRARY_DEBUG geUtild)
find_library(Assimp_LIBRARY_RELEASE assimp-vc140-mt)
find_library(Assimp_LIBRARY_DEBUG assimp-vc140-mt)

set(GLSG_FILES
   src/glsg/EnumToGL.h 
   src/glsg/EnumToGL.cpp
   src/glsg/GLAttribute.h
   src/glsg/GLScene.h
   src/glsg/GLSceneProcessor.h 
   src/glsg/GLSceneProcessor.cpp
)

set(SHADER_FILES
   resources/shaders/VS.glsl 
   resources/shaders/FS.glsl 
)

set(GUI_FILES
   src/GUI.qml 
)

set(RESOURCE_FILES
   src/resources.qrc 
)

set(HEADER_FILES
   src/Renderer.h
   src/RendererQQuickItem.h
   src/SimpleVT.h
   src/VisualizationTechnique.h
   src/AABB.h
   src/SceneLoadingHandler.h
   src/PointLight.h
   src/Application.h
   src/LightsGenerationHandler.h
   src/Scene.h
   src/Camera.h
   src/CameraSettingsHandler.h
   src/MouseEventHandler.h
   src/KeyEventHandler.h
   ${QtImageLoader_INCLUDE_DIR}/QtImageLoader.h
   ${AssimpModelLoader_INCLUDE_DIR}/AssimpModelLoader.h
   ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOStream.h
   ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOSystem.h
)

if(WIN32)
   set(SOURCE_FILES
       src/main.cpp
       src/Renderer.cpp
       src/RendererQQuickItem.cpp
       src/SimpleVT.cpp
       src/AABB.cpp
       src/SceneLoadingHandler.cpp
       src/Application.cpp
	   src/LightsGenerationHandler.cpp
	   src/Scene.cpp
	   src/Camera.cpp
	   src/CameraSettingsHandler.cpp
	   src/MouseEventHandler.cpp
	   src/KeyEventHandler.cpp
       ${QtImageLoader_INCLUDE_DIR}/QtImageLoader.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/AssimpModelLoader.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOStream.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOSystem_win32.cpp
   )
else()
   set(SOURCE_FILES
       src/main.cpp
       src/Renderer.cpp
       src/RendererQQuickItem.cpp
       src/SimpleVT.cpp
       src/AABB.cpp
       src/SceneLoadingHandler.cpp
       src/Application.cpp
	   src/LightsGenerationHandler.cpp
	   src/Scene.cpp
	   src/Camera.cpp
	   src/CameraSettingsHandler.cpp
	   src/MouseEventHandler.cpp
	   src/KeyEventHandler.cpp
       ${QtImageLoader_INCLUDE_DIR}/QtImageLoader.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/AssimpModelLoader.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOStream.cpp
       ${AssimpModelLoader_INCLUDE_DIR}/UnicodeIOSystem_other.cpp
   )
endif(WIN32)

# Define a grouping for source files in IDE project generation
source_group(glsg FILES ${GLSG_FILES})

# Define a grouping for source files in IDE project generation
source_group("Shader Files" FILES ${SHADER_FILES})

# Define a grouping for source files in IDE project generation
source_group("GUI Files" FILES ${GUI_FILES})

# Define a grouping for source files in IDE project generation
source_group("Resource Files" FILES ${RESOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Source Files" FILES ${SOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Header Files" FILES ${HEADER_FILES})

set(DEFAULT_RESOERCES_PATH "${CMAKE_CURRENT_LIST_DIR}/resources")
set(${APP_NAME}_RESOURCES "${DEFAULT_RESOERCES_PATH}" CACHE PATH "Relative or absolute path to Application resources.")

add_executable(${APP_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${GUI_FILES} ${RESOURCE_FILES} ${GLSG_FILES} ${SHADER_FILES})

set_target_properties(${APP_NAME} PROPERTIES COMPILE_DEFINITIONS "APP_RESOURCES=\"${${APP_NAME}_RESOURCES}\"")

target_include_directories(${APP_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src 
                                       PUBLIC ${GPUEngine_INCLUDE_DIR} 
                                       PUBLIC ${Assimp_INCLUDE_DIR} 
                                       PUBLIC ${AssimpModelLoader_INCLUDE_DIR}
                                       PUBLIC ${QtImageLoader_INCLUDE_DIR}
                                       PUBLIC ${ste_INCLUDE_DIR})                                     

target_link_libraries(${APP_NAME} Qt5::Quick 
                                  Qt5::Concurrent
                                  glm
                                  optimized ${geCore_LIBRARY_RELEASE}
                                  optimized ${geGL_LIBRARY_RELEASE}
                                  optimized ${geSG_LIBRARY_RELEASE}
                                  optimized ${geUtil_LIBRARY_RELEASE}
                                  optimized ${Assimp_LIBRARY_RELEASE}
                                  debug ${geCore_LIBRARY_DEBUG}
                                  debug ${geGL_LIBRARY_DEBUG}
                                  debug ${geSG_LIBRARY_DEBUG}
                                  debug ${geUtil_LIBRARY_DEBUG}
                                  debug ${Assimp_LIBRARY_DEBUG})
              
