#include "D3D11TextureManager.h"
#include "D3D11RenderApi.h"

namespace Hollow {
	s_ptr<Texture> D3D11TextureManager::create2dTexture(const s_ptr<Import::Texture>& desc)
	{
		HW_INFO("D3D11TextureManager: creating 2d texture, filename {} bytes {}", desc->name.c_str(), desc->size);
		D3D11Texture* texture = new D3D11Texture();

		texture->active = true;

		D3D11RenderApi* r = static_cast<D3D11RenderApi*>(RenderApi::instance());
		ID3D11Device* device = r->getContext().getDevice();
		ID3D11DeviceContext* deviceContext = r->getContext().getDeviceContext();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Height = desc->height;
		textureDesc.Width = desc->width;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format = D3D11Helper::getFormat(desc->format);
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		textureDesc.MipLevels = 5;

		//// If texture is created for compute shader
		//if (desc->unorderedAccess) {
		//	textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		//}

		if (desc->data != nullptr) {
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = desc->data.get();
			initData.SysMemPitch = desc->pitch;
			initData.SysMemSlicePitch = 0;

			if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &texture->m_Texture))) {
				HW_ERROR("D3DTexture: Can't create 2D texture");
				return nullptr;
			}

			D3D11_BOX box;
			box.left = 0;
			box.right = desc->width;
			box.bottom = desc->height;
			box.top = 0;
			box.front = 0;
			box.back = 1;

			deviceContext->UpdateSubresource(texture->m_Texture, 0, &box, desc->data.get(), desc->pitch, 1);
		}
		else {
			if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &texture->m_Texture))) {
				HW_ERROR("D3DTexture: Can't create 2D texture");
				return nullptr;
			}
		}

		/*if (desc->unorderedAccess) {
			D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
			ZeroMemory(&descUAV, sizeof(descUAV));
			descUAV.Format = textureDesc.Format;
			descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			descUAV.Texture2D.MipSlice = 0;
			if (FAILED(device->CreateUnorderedAccessView(texture->m_Texture, &descUAV, &texture->m_UnorderedAccessView))) {
				HW_ERROR("D3DTexture: Can't create unorderered access view");
				return nullptr;
			}
		}*/

		// Setup the shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 5;
		
		if (device->CreateShaderResourceView(texture->m_Texture, &srvDesc, &texture->m_TextureShaderResource) != S_OK) {
			HW_ERROR("D3DTexture: Can't create shader resource view for 2d texture");
			return nullptr;
		}

		deviceContext->GenerateMips(texture->m_TextureShaderResource);

		s_ptr<Texture> texturePtr(texture);
		textureList[desc->name] = texturePtr;

		return texturePtr;
	}

	s_ptr<Texture> D3D11TextureManager::create3dTexture(const std::vector<s_ptr<Import::Texture>>& desc)
	{
		HW_INFO("D3D11TextureManager: creating 3d texture, filenames {} {} {} {} {} {} ", desc[0]->name.c_str(), desc[1]->name.c_str(), desc[2]->name.c_str(), desc[3]->name.c_str(), desc[4]->name.c_str(), desc[5]->name.c_str());
		D3D11Texture* texture = new D3D11Texture();

		texture->active = true;

		D3D11RenderApi* r = static_cast<D3D11RenderApi*>(RenderApi::instance());
		ID3D11Device* device = r->getContext().getDevice();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Height = desc[0]->height;
		textureDesc.Width = desc[0]->width;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA pData[6];

		for (int i = 0; i < 6; i++) {
			pData[i].pSysMem = desc[i]->data.get();
			pData[i].SysMemPitch = desc[i]->pitch;
			pData[i].SysMemSlicePitch = 0;
		}

		if (device->CreateTexture2D(&textureDesc, pData, &texture->m_Texture) != S_OK) {
			HW_ERROR("D3DTexture: Can't create 2D texture");
			return nullptr;
		}

		// Setup the shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view for the texture.
		if (device->CreateShaderResourceView(texture->m_Texture, &srvDesc, &texture->m_TextureShaderResource) != S_OK) {
			HW_ERROR("D3DTexture: Can't create shader resource view for 2d texture");
			return nullptr;
		}

		s_ptr<Texture> texturePtr(texture);
		textureList[desc[0]->name] = texturePtr;

		return texturePtr;
	}

	s_ptr<Texture> D3D11TextureManager::create3dTexture(const s_ptr<Import::Texture>& desc)
	{
		D3D11Texture* texture = new D3D11Texture();

		int xOffset = desc->width / 4;
		int yOffset = desc->height / 3;
		std::array<unsigned char*, 6> data;
		for (int i = 0; i < data.size(); i++) {
			data[i] = new unsigned char[xOffset * yOffset * 4];
		}

		unsigned char* textureData = (unsigned char*)desc->data.get();

		// forward 1 1
		// ? * xOffset * 4 - width | yOffset * desc->width * ? * 4 - plane
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[4][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 0];
				data[4][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 1];
				data[4][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 2];
				data[4][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		// back  2 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[5][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 0];
				data[5][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 1];
				data[5][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 2];
				data[5][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (3 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		// up 1 0
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[2][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 0 * 4) + (oy * desc->width * 4) + 0];
				data[2][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 0 * 4) + (oy * desc->width * 4) + 1];
				data[2][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 0 * 4) + (oy * desc->width * 4) + 2];
				data[2][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 0 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		// down 1 2
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[3][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 2 * 4) + (oy * desc->width * 4) + 0];
				data[3][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 2 * 4) + (oy * desc->width * 4) + 1];
				data[3][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 2 * 4) + (oy * desc->width * 4) + 2];
				data[3][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (1 * xOffset * 4) + (yOffset * desc->width * 2 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		// right 2 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[0][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 0];
				data[0][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 1];
				data[0][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 2];
				data[0][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (2 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		// left 0 1
		for (int oy = 0; oy < yOffset; oy++) {
			for (int ox = 0; ox < xOffset; ox++) {
				data[1][ox * 4 + oy * yOffset * 4 + 0] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 0];
				data[1][ox * 4 + oy * yOffset * 4 + 1] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 1];
				data[1][ox * 4 + oy * yOffset * 4 + 2] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 2];
				data[1][ox * 4 + oy * yOffset * 4 + 3] = textureData[(ox * 4) + (0 * xOffset * 4) + (yOffset * desc->width * 1 * 4) + (oy * desc->width * 4) + 3];
			}
		}

		
		D3D11RenderApi* r = static_cast<D3D11RenderApi*>(RenderApi::instance());
		ID3D11Device* device = r->getContext().getDevice();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Height = desc->height / 3;
		textureDesc.Width = desc->width / 4;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA pData[6];

		for (int i = 0; i < 6; i++) {
			pData[i].pSysMem = data[i];
			pData[i].SysMemPitch = xOffset * 4;
			pData[i].SysMemSlicePitch = 0;
		}

		if (device->CreateTexture2D(&textureDesc, pData, &texture->m_Texture) != S_OK) {
			HW_ERROR("D3DTexture: Can't create 2D texture");
			return nullptr;
		}

		// Setup the shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view for the texture.
		if (device->CreateShaderResourceView(texture->m_Texture, &srvDesc, &texture->m_TextureShaderResource) != S_OK) {
			HW_ERROR("D3DTexture: Can't create shader resource view for 2d texture");
			return nullptr;
		}

		for (int i = 0; i < data.size(); i++) {
			delete[] data[i];
		}

		s_ptr<Texture> texturePtr(texture);
		textureList[desc->name] = texturePtr;

		return texturePtr;
	}
}