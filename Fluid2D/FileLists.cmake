SET(FLUID2D_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Fluid2D)

SET(FLUID2D_HEADERS
    ${FLUID2D_SRC_DIR}/FluidCharacter.h)
    
SET(FLUID2D_SOURCES
    ${FLUID2D_SRC_DIR}/FluidCharacter.cpp)

SET(FLUID2D_RCC_FILES
    ${FLUID2D_SRC_DIR}/resources/Fluid2D.qrc)
SET(FLUID2D_SHADER_FILES
    ${FLUID2D_SRC_DIR}/resources/shaders/advect.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/divergence.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/drawFluid.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/drawFluid.vert
    ${FLUID2D_SRC_DIR}/resources/shaders/frontier.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/gradSub.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/heat.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/jacobi.frag
    ${FLUID2D_SRC_DIR}/resources/shaders/update.vert)
SET(FLUID2D_TEXTURES_FILES
    ${FLUID2D_SRC_DIR}/resources/textures/statsPanel.bmp)
SET(FLUID2D_RESOURCES
    ${FLUID2D_RCC_FILES}
    ${FLUID2D_SHADER_FILES}
    ${FLUID2D_TEXTURES_FILES})
QT5_ADD_RESOURCES(FLUID2D_RCC_SRCS
    ${FLUID2D_RCC_FILES})


SET(FLUID2D_SRC_FILES
    ${FLUID2D_HEADERS}
    ${FLUID2D_SOURCES}
    ${FLUID2D_RCC_SRCS}
    ${FLUID2D_RESOURCES})


# Visual Studio filters
SOURCE_GROUP("Header Files" FILES ${FLUID2D_HEADERS})
SOURCE_GROUP("Source Files" FILES ${FLUID2D_SOURCES})
SOURCE_GROUP("Config" FILES ${FLUID2D_CONFIG_FILES})
