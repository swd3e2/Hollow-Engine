cbuffer ConstantBuffer : register(b0)
{
	matrix WVP;
}

cbuffer ConstantBuffer : register(b2)
{
	matrix transform;
	bool hasAnimation;
}

cbuffer ConstantBuffer : register(b7)
{
	matrix boneInfo[100];
}

struct PixelShaderOutput
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL0;
};

struct VertexShaderInput
{
	float3 pos			: POSITION;
	float2 texCoord		: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 bitangent	: BITANGENT;
	int4   boneId		: BONEID;
	float4 weight		: WEIGHT;
};

PixelShaderOutput main(VertexShaderInput input)
{
	PixelShaderOutput output;
	output.pos = float4(input.pos, 1.0f);
	
	if (hasAnimation) {
		matrix BoneTransform = boneInfo[input.boneId.x] * input.weight.x;
		BoneTransform += boneInfo[input.boneId.y] * input.weight.y;
		BoneTransform += boneInfo[input.boneId.z] * input.weight.z;
		BoneTransform += boneInfo[input.boneId.w] * input.weight.w;

		output.pos = mul(output.pos, BoneTransform);
		output.normal = mul(input.normal, BoneTransform);
	}
	

	output.pos = mul(output.pos, WVP);
	output.texCoord = input.texCoord;

	return output;
}