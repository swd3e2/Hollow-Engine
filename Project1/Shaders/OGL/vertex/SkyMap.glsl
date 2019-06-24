#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in vec4 boneIDs;
layout(location = 6) in vec4 weights;

out VS_OUT
{
	vec3 texCoord;
} vs_out;

layout(std140, binding = 0) uniform Matrices
{
	mat4 WVP;
	vec3 cameraPosition;
};

void main()
{
	vs_out.texCoord = pos;
	vec4 cpos = vec4(pos, 1.0) * WVP;
	gl_Position = cpos.xyww;
}