#pragma once

#ifndef HW_TEXTURE_MANAGER_H
#define HW_TEXTURE_MANAGER_H

#include "Hollow/Common/Log.h"
#include "Hollow/Core/CModule.h"
#include "Hollow/Importer/FreeImgImporter.h"
#include "Base/Texture.h"
#include <unordered_map>
#include <string>
#include "Hollow/Platform.h"

namespace Hollow {
	class TextureManager : public CModule<TextureManager>
	{
	public:
		std::unordered_map<std::string, Texture*> textureList;
		std::unordered_map<Texture*, size_t> textureRefCnt;
		const std::string baseTexturePapth = "C:/dev/Hollow Engine/Sandbox/Sandbox/Resources/Textures/";
	public:
		Texture* createTextureFromFile(const std::string& filename, bool fromDefaultFolder = true);
		void remove(Texture* texture);
		void removeAll();
		virtual Texture* create2dTexture(TextureData* desc) = 0;
		virtual Texture* create3dTexture(TextureData** desc) = 0;
		virtual Texture* create3dTexture(TextureData* desc) = 0;
	};
}

#endif