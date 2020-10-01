#--------------------------------------------------------------------------
# example project
#--------------------------------------------------------------------------

PROJECT(Textures)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("Textures" FILES ${files_example})

ADD_EXECUTABLE(Textures ${files_example})
TARGET_LINK_LIBRARIES(Textures core render)
ADD_DEPENDENCIES(Textures core render)

SET_PROPERTY(TARGET Textures PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")