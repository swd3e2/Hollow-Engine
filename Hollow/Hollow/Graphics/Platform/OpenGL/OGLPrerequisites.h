#pragma once

#ifndef OGL_LIBS
#define OGL_LIBS
	#pragma comment ( lib, "opengl32.lib" )
	#pragma comment ( lib, "glu32.lib" )
#endif

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include "GL/glew.h"
#endif

namespace Hollow {
	class OGLRenderApi;
	class OGLHardwareBufferManager;
}