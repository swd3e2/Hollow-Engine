#pragma once

#include <memory>
#include "Hollow/Platform.h"

enum TextureFormat
{
	FORMAT_B8G8R8A8_UNORM,
	FORMAT_R32G32B32A32
};

enum TextureType
{
	TEXTURE2D,
	TEXTURE_CUBE
};

class HOLLOW_API TEXTURE_DESC
{
public:
	int width;
	int height;
	int pitch;
	void* mInitialData;
	bool unorderedAccess;
	TextureFormat format;
	TextureType type;
public:
	TEXTURE_DESC() :
		format(FORMAT_B8G8R8A8_UNORM), unorderedAccess(false)
	{}
	~TEXTURE_DESC()
	{
		if (mInitialData != nullptr) {
			free(mInitialData);
		}
	}
};

class Texture
{

};