#pragma once

#include "OGLInputLayout.h"
#include "Hollow/Graphics/InputLayoutManager.h"
#include "OGLHelper.h"

namespace Hollow {
	class OGLInputLayoutManager : public InputLayoutManager
	{
	public:
		virtual s_ptr<InputLayout> create(const INPUT_LAYOUT_DESC& desc) override;
	};
}