#pragma once
#include <d3d11.h>
#include "Hollow/Common/Log.h"
#include <wrl/client.h>

class D3DRasterizerState
{
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		m_RasterizerState;
public:
	D3DRasterizerState(ID3D11Device* device)
	{
		HRESULT hr = S_OK;

		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		hr = device->CreateRasterizerState(&rasterizerDesc, m_RasterizerState.GetAddressOf());

		if (hr != S_OK) {
			Hollow::Log::GetCoreLogger()->error("RenderSystem: Cant create RasterizerState!");
		}
	}

	ID3D11RasterizerState* GetRasterizerState()
	{
		return this->m_RasterizerState.Get();
	}
};