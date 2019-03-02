#pragma once
#include "Hollow/Graphics/IMaterial.h"
#include "D3DTexture.h"

class D3DMaterial : public IMaterial
{
public:
	void SetDiffuseTexture(D3DTexture* texture) { diffuseTexture = texture; }
	D3DTexture* diffuseTexture;
};