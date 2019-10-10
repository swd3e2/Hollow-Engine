#include "OGLTextureManager.h"

namespace Hollow {
	s_ptr<Texture> OGLTextureManager::create2dTexture(const s_ptr<Import::Texture>& texture, const TEXTURE_DESC& desc)
	{
		OGLTexture* oglTexture = new OGLTexture(texture->width, texture->height);

		glGenTextures(1, &oglTexture->textureId);
		glBindTexture(GL_TEXTURE_2D, oglTexture->textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->data.get());

		glActiveTexture(GL_TEXTURE1);
		glGenerateMipmap(GL_TEXTURE_2D);

		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

		glBindTexture(GL_TEXTURE_2D, 0);

		s_ptr<Texture> texturePtr(oglTexture);
		textureList[texture->name] = texturePtr;

		return texturePtr;
	}

	s_ptr<Texture> OGLTextureManager::create2dTexture(const TEXTURE_DESC& desc)
	{
		OGLTexture* oglTexture = new OGLTexture(desc.width, desc.height);

		glGenTextures(1, &oglTexture->textureId);
		glBindTexture(GL_TEXTURE_2D, oglTexture->textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc.width, desc.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->data.get());

		glActiveTexture(GL_TEXTURE1);
		glGenerateMipmap(GL_TEXTURE_2D);

		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

		glBindTexture(GL_TEXTURE_2D, 0);

		s_ptr<Texture> texturePtr(oglTexture);
		textureList[texture->name] = texturePtr;

		return texturePtr;
	}

	s_ptr<Texture> OGLTextureManager::create3dTexture(const std::vector<s_ptr<Import::Texture>>& texture, const TEXTURE_DESC& desc)
	{
		HW_INFO("OGLTextureManager: creating 3d texture, filenames {} {} {} {} {} {} ", 
			texture[0]->name.c_str(), 
			texture[1]->name.c_str(), 
			texture[2]->name.c_str(), 
			texture[3]->name.c_str(), 
			texture[4]->name.c_str(), 
			texture[5]->name.c_str()
		);
		OGLTexture* oglTexture = new OGLTexture(texture[0]->width, texture[0]->height);
		oglTexture->type = TextureType::TT_TEXTURE_CUBE;

		glGenTextures(1, &oglTexture->textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, oglTexture->textureId);
		for (int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texture[i]->width, texture[i]->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture[i]->data.get());
		}

		/*glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);*/

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


		s_ptr<Texture> texturePtr(oglTexture);
		textureList[texture[0]->name] = texturePtr;

		return texturePtr;
	}
	s_ptr<Texture> OGLTextureManager::create3dTexture(const s_ptr<Import::Texture>& texture, const TEXTURE_DESC& desc)
	{
		int xOffset = texture->width / 4;
		int yOffset = texture->height / 3;
		std::array<unsigned char*, 6> data;
		for (int i = 0; i < data.size(); i++) {
			data[i] = new unsigned char[xOffset * yOffset * 4];
		}

		unsigned char* textureData = (unsigned char*)texture->data.get();

		OGLTexture* oglTexture = new OGLTexture(xOffset, yOffset);
		oglTexture->type = TextureType::TT_TEXTURE_CUBE;

		// forward 1 1
		// ? * xOffset * 4 - width | yOffset * desc->width * ? * 4 - plane
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[4][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 0];
				data[4][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 1];
				data[4][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 2];
				data[4][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 3];
			}
		}

		// back  2 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[5][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 0];
				data[5][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 1];
				data[5][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 2];
				data[5][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 3];
			}
		}

		// up 1 0
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[2][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 0 * 4) + (oy * texture->width * 4) + 0];
				data[2][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 0 * 4) + (oy * texture->width * 4) + 1];
				data[2][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 0 * 4) + (oy * texture->width * 4) + 2];
				data[2][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 0 * 4) + (oy * texture->width * 4) + 3];
			}
		}

		// down 1 2
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[3][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 2 * 4) + (oy * texture->width * 4) + 0];
				data[3][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 2 * 4) + (oy * texture->width * 4) + 1];
				data[3][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 2 * 4) + (oy * texture->width * 4) + 2];
				data[3][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * texture->width * 2 * 4) + (oy * texture->width * 4) + 3];
			}
		}

		// right 2 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[0][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 0];
				data[0][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 1];
				data[0][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 2];
				data[0][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 3];
			}
		}

		// left 0 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[1][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 0];
				data[1][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 1];
				data[1][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 2];
				data[1][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * texture->width * 1 * 4) + (oy * texture->width * 4) + 3];
			}
		}


		glGenTextures(1, &oglTexture->textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, oglTexture->textureId);
		for (int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, xOffset, yOffset, 0, GL_BGRA, GL_UNSIGNED_BYTE, data[i]);
		}

		/*glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);*/

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		for (int i = 0; i < data.size(); i++) {
			delete[] data[i];
		}

		s_ptr<Texture> texturePtr(oglTexture);
		textureList[texture->name] = texturePtr;

		return texturePtr;
	}
	s_ptr<Texture> OGLTextureManager::create3dTexture(const TEXTURE_DESC& desc)
	{
		return s_ptr<Texture>();
	}
}