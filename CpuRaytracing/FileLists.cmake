SET(CPURAYTRACING_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CpuRaytracing)

## HEADERS ##
SET(CPURAYTRACING_MANAGERS_HEADERS
    ${CPURAYTRACING_SRC_DIR}/Managers/AnimationManager.h
    ${CPURAYTRACING_SRC_DIR}/Managers/TimelineManager.h
    ${CPURAYTRACING_SRC_DIR}/Managers/PostProdManager.h
    ${CPURAYTRACING_SRC_DIR}/Managers/CameraManager.h
    ${CPURAYTRACING_SRC_DIR}/Managers/PathManager.h)

SET(CPURAYTRACING_MODEL_HEADERS
    ${CPURAYTRACING_SRC_DIR}/Model/PathModel.h
    ${CPURAYTRACING_SRC_DIR}/Model/PathReader.h
    ${CPURAYTRACING_SRC_DIR}/Model/PathWriter.h
    ${CPURAYTRACING_SRC_DIR}/Model/SceneDocument.h)

SET(CPURAYTRACING_HEADERS
    ${CPURAYTRACING_SRC_DIR}/CpuRaytracingCharacter.h
    ${CPURAYTRACING_SRC_DIR}/TheFruitChoreographer.h
    ${CPURAYTRACING_SRC_DIR}/RaytracedView.h
    ${CPURAYTRACING_SRC_DIR}/RaytracerGui.h)
    

## SOURCES ##
SET(CPURAYTRACING_MANAGERS_SOURCES
    ${CPURAYTRACING_SRC_DIR}/Managers/AnimationManager.cpp
    ${CPURAYTRACING_SRC_DIR}/Managers/TimelineManager.cpp
    ${CPURAYTRACING_SRC_DIR}/Managers/PostProdManager.cpp
    ${CPURAYTRACING_SRC_DIR}/Managers/CameraManager.cpp
    ${CPURAYTRACING_SRC_DIR}/Managers/PathManager.cpp)

SET(CPURAYTRACING_MODEL_SOURCES
    ${CPURAYTRACING_SRC_DIR}/Model/PathModel.cpp
    ${CPURAYTRACING_SRC_DIR}/Model/PathReader.cpp
    ${CPURAYTRACING_SRC_DIR}/Model/SceneDocument.cpp)

SET(CPURAYTRACING_SOURCES
    ${CPURAYTRACING_SRC_DIR}/CpuRaytracingCharacter.cpp
    ${CPURAYTRACING_SRC_DIR}/TheFruitChoreographer.cpp
    ${CPURAYTRACING_SRC_DIR}/TheFruitAnimation.cpp
    ${CPURAYTRACING_SRC_DIR}/RaytracedView.cpp
    ${CPURAYTRACING_SRC_DIR}/RaytracerGui.cpp)


## Resources ##
QT5_ADD_RESOURCES(CPURAYTRACING_RESOURCES
    ${CPURAYTRACING_SRC_DIR}/resources/CpuRaytracing.qrc)


## UI
SET(CPURAYTRACING_UI_FILES
    ${CPURAYTRACING_SRC_DIR}/RaytracerGui.ui)
QT5_WRAP_UI(CPURAYTRACING_UI_SRCS ${CPURAYTRACING_UI_FILES})



## GLOBAL ##
SET(CPURAYTRACING_SRC_FILES
    ${CPURAYTRACING_MANAGERS_HEADERS}
    ${CPURAYTRACING_MODEL_HEADERS}
    ${CPURAYTRACING_HEADERS}
    ${CPURAYTRACING_MANAGERS_SOURCES}
    ${CPURAYTRACING_MODEL_SOURCES}
    ${CPURAYTRACING_SOURCES}
    ${CPURAYTRACING_RESOURCES}
    ${CPURAYTRACING_UI_FILES}
    ${CPURAYTRACING_UI_SRCS})
