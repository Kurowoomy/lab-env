#--------------------------------------------------------------------------
# example project
#--------------------------------------------------------------------------

PROJECT(ShaderResources)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("ShaderResources" FILES ${files_example})

ADD_EXECUTABLE(ShaderResources ${files_example})
TARGET_LINK_LIBRARIES(ShaderResources core render)
ADD_DEPENDENCIES(ShaderResources core render)