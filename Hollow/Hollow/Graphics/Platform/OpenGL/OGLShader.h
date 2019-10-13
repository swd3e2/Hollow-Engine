#pragma once

#ifndef HW_OGL_SHADER_H
#define HW_OGL_SHADER_H

#include "Hollow/Graphics/Shader.h"
#include "OGLPrerequisites.h"
#define DEFAULT_SHADER_ID -1

namespace Hollow {
	class OGLShader : public Shader
	{
	public:
		int shaderId;
		int prevShaderId;

	public:
		~OGLShader()
		{
			release();
		}

		virtual void release() override
		{
			if (shaderId != DEFAULT_SHADER_ID) {
				glDeleteShader(shaderId);
			}
		}
	};
}

#endif