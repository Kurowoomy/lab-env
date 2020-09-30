#--------------------------------------------------------------------------
# example project
#--------------------------------------------------------------------------

PROJECT(ObjectFiles)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("ObjectFiles" FILES ${files_example})

ADD_EXECUTABLE(ObjectFiles ${files_example})
TARGET_LINK_LIBRARIES(ObjectFiles core render)
ADD_DEPENDENCIES(ObjectFiles core render)