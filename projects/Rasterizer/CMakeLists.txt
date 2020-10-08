#--------------------------------------------------------------------------
# example project
#--------------------------------------------------------------------------

PROJECT(Rasterizer)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("Rasterizer" FILES ${files_example})

ADD_EXECUTABLE(Rasterizer ${files_example})
TARGET_LINK_LIBRARIES(Rasterizer core render)
ADD_DEPENDENCIES(Rasterizer core render)

SET_PROPERTY(TARGET Rasterizer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")