SET(VOLUME_RENDERING_SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/VolumeRendering)

SET(VOLUME_RENDERING_HEADERS
    ${VOLUME_RENDERING_SRC_DIR}/Lights.h
    ${VOLUME_RENDERING_SRC_DIR}/Volumes.h
    ${VOLUME_RENDERING_SRC_DIR}/Visualizer.h)
    
SET(VOLUME_RENDERING_SOURCES
    ${VOLUME_RENDERING_SRC_DIR}/Lights.cpp
    ${VOLUME_RENDERING_SRC_DIR}/Volumes.cpp
    ${VOLUME_RENDERING_SRC_DIR}/Visualizer.cpp)

SET(FRACTAL_SHADERS_SRC
    ${VOLUME_RENDERING_SRC_DIR}/resources/shaders/render.vert
    ${VOLUME_RENDERING_SRC_DIR}/resources/shaders/render.frag
    ${VOLUME_RENDERING_SRC_DIR}/resources/shaders/env.vert
    ${VOLUME_RENDERING_SRC_DIR}/resources/shaders/env.frag)

SET(FRACTAL_TEXTURES_SRC
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_x-.png
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_x+.png
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_y-.png
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_y+.png
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_z-.png
    ${VOLUME_RENDERING_SRC_DIR}/resources/textures/Museum_CubeMap_z+.png)
    
## Resources ##
QT5_ADD_RESOURCES(VOLUME_RENDERING_RESOURCES
    ${VOLUME_RENDERING_SRC_DIR}/resources/VolumeRendering.qrc)
    
## Global ##
SET(VOLUME_RENDERING_SRC_FILES
    ${VOLUME_RENDERING_HEADERS}
    ${VOLUME_RENDERING_SOURCES}
    ${VOLUME_RENDERING_RESOURCES}
    ${VOLUME_RENDERING_CONFIG_FILES})
    

