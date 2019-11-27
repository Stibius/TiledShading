cmake_minimum_required (VERSION 3.8)

# solution
project(TiledShading)
# project
set(APP_NAME "TiledShading")

set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 QUIET COMPONENTS Quick Concurrent)
find_package(GPUEngine QUIET COMPONENTS geCore geGL geSG geUtil)

find_package(Assimp REQUIRED QUIET)  

find_path(Assimp_INCLUDE_DIR assimp/config.h HINTS ${Assimp_DIR}/../../../include)
find_library(Assimp_LIBRARY ${ASSIMP_LIBRARIES} HINTS ${Assimp_DIR}/../../../lib)

set(STE_FILES
   src/ste/DAG.h 
   src/ste/stl_extension.h
   src/ste/concepts.h
)

set(QT_IMAGE_LOADER_FILES
   src/QtImageLoader/QtImageLoader.h 
   src/QtImageLoader/QtImageLoader.cpp
)

set(ASSIMP_MODEL_LOADER_FILES
   src/AssimpModelLoader/AssimpModelLoader.h
   src/AssimpModelLoader/UnicodeIOStream.h
   src/AssimpModelLoader/UnicodeIOSystem.h
   src/AssimpModelLoader/AssimpModelLoader.cpp
   src/AssimpModelLoader/UnicodeIOStream.cpp
)

if(WIN32)
   set(ASSIMP_MODEL_LOADER_FILES
      ${ASSIMP_MODEL_LOADER_FILES}
      src/AssimpModelLoader/UnicodeIOSystem_win32.cpp
   )
else()
   set(ASSIMP_MODEL_LOADER_FILES
      ${ASSIMP_MODEL_LOADER_FILES}
      src/AssimpModelLoader/UnicodeIOSystem_other.cpp
   )    
endif(WIN32)

set(GLSG_FILES
   src/glsg/EnumToGL.h 
   src/glsg/EnumToGL.cpp
   src/glsg/GLAttribute.h
   src/glsg/GLScene.h
   src/glsg/GLSceneProcessor.h 
   src/glsg/GLSceneProcessor.cpp
)

set(SHADER_FILES
   src/shaders/forwardVS.glsl 
   src/shaders/forwardFS.glsl 
   src/shaders/deferredGeometryVS.glsl 
   src/shaders/deferredGeometryFS.glsl 
   src/shaders/deferredLightingVS.glsl 
   src/shaders/deferredLightingFS.glsl 
   src/shaders/tiledDeferredRenderVS.glsl 
   src/shaders/tiledDeferredRenderFS.glsl
   src/shaders/tiledDeferredLightingCS.glsl
)

set(GUI_FILES
   src/GUI.qml 
)

set(RESOURCE_FILES
   src/resources.qrc 
)

set(HEADER_FILES
   src/AABB.h
   src/Application.h
   src/Camera.h
   src/CameraSettingsHandler.h
   src/DeferredShadingVT.h
   src/FileLoader.h
   src/ForwardShadingVT.h
   src/GBuffer.h
   src/GBufferVT.h
   src/KeyEventHandler.h
   src/LightedSceneVT.h
   src/LightsGenerationHandler.h
   src/MouseEventHandler.h
   src/PointLight.h
   src/Renderer.h
   src/RendererQQuickItem.h
   src/RenderingSettingsHandler.h
   src/Scene.h
   src/SceneLoadingHandler.h
   src/SceneVT.h
   src/Sphere.h
   src/TiledDeferredShadingVT.h 
   src/VisualizationTechnique.h
)

set(SOURCE_FILES
   src/AABB.cpp
   src/Application.cpp
   src/Camera.cpp
   src/CameraSettingsHandler.cpp
   src/DeferredShadingVT.cpp
   src/FileLoader.cpp
   src/ForwardShadingVT.cpp
   src/GBuffer.cpp
   src/GBufferVT.cpp
   src/KeyEventHandler.cpp
   src/LightedSceneVT.cpp
   src/LightsGenerationHandler.cpp
   src/main.cpp
   src/MouseEventHandler.cpp
   src/Renderer.cpp
   src/RendererQQuickItem.cpp
   src/RenderingSettingsHandler.cpp
   src/Scene.cpp
   src/SceneLoadingHandler.cpp
   src/SceneVT.cpp
   src/TiledDeferredShadingVT.cpp 
)

# Define a grouping for source files in IDE project generation
source_group("ste" FILES ${STE_FILES})

# Define a grouping for source files in IDE project generation
source_group("QtImageLoader" FILES ${QT_IMAGE_LOADER_FILES})

# Define a grouping for source files in IDE project generation
source_group("AssimpModelLoader" FILES ${ASSIMP_MODEL_LOADER_FILES})

# Define a grouping for source files in IDE project generation
source_group("glsg" FILES ${GLSG_FILES})

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

add_executable(${APP_NAME} ${SOURCE_FILES} 
                           ${HEADER_FILES} 
                           ${GUI_FILES} 
                           ${RESOURCE_FILES} 
                           ${GLSG_FILES} 
                           ${STE_FILES} 
                           ${QT_IMAGE_LOADER_FILES} 
                           ${ASSIMP_MODEL_LOADER_FILES} 
                           ${SHADER_FILES})

set_target_properties(${APP_NAME} PROPERTIES COMPILE_DEFINITIONS "APP_RESOURCES=\"${${APP_NAME}_RESOURCES}\"")

target_include_directories(${APP_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src 
                                       PUBLIC ${PROJECT_SOURCE_DIR}/src/QtImageLoader 
                                       PUBLIC ${PROJECT_SOURCE_DIR}/src/AssimpModelLoader 
                                       PUBLIC ${Assimp_INCLUDE_DIR} 
                                       )                                     

target_link_libraries(${APP_NAME} Qt5::Quick 
                                  Qt5::Concurrent
                                  geCore
                                  geGL
                                  geSG
                                  geUtil
                                  ${Assimp_LIBRARY}
                                  )
              
