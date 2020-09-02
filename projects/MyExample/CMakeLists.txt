#--------------------------------------------------------------------------
# example project
#--------------------------------------------------------------------------

PROJECT(MyExample)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("MyExample" FILES ${files_example})

ADD_EXECUTABLE(MyExample ${files_example})
TARGET_LINK_LIBRARIES(MyExample core render)
ADD_DEPENDENCIES(MyExample core render)