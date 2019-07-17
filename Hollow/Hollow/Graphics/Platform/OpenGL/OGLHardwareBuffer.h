#pragma once

#include "Hollow/Graphics/Base/HardwareBuffer.h"
#include "OGLPrerequisites.h"
#include "OGLInputLayout.h"

namespace Hollow {
	class OGLHardwareBuffer : public HardwareBuffer
	{
	public:
		GLuint mVertexArrayObject;
		GLuint mVertexArrayBuffer;
		size_t vboSize;
		InputLayout* mCurrentInputLayout;
	public:
		OGLHardwareBuffer(size_t size, size_t stride) : HardwareBuffer(size, stride) {}
	};

	class OGLIndexBuffer : public IndexBuffer
	{

	};

	class OGLVertexBuffer : public VertexBuffer
	{

	};
}