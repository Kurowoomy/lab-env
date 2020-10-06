#--------------------------------------------------------------------------
# render
#--------------------------------------------------------------------------

SET(files_render_display
	window.h
	window.cc
	MeshResource.h
	MeshResource.cc
	TextureResource.h
	TextureResource.cc
	stb_image.h
	stb_image.cc
	ShaderObject.h
	ShaderObject.cc
	ShaderVertex.txt
	ShaderFragment.txt
	GraphicsNode.h
	GraphicsNode.cc
	PointLight.h
	PointLight.cc)
SOURCE_GROUP("display" FILES ${files_render_display})


SET(files_render
	${files_render_display}
	${files_render_pch})

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(render STATIC ${files_render} ${files_pch})
TARGET_PCH(render ../)	
ADD_DEPENDENCIES(render glew glfw)
TARGET_LINK_LIBRARIES(render PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS})