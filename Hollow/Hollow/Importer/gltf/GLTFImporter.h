#pragma once

#include "tiny_gltf.h"

#include "Hollow/Graphics/Vertex.h"
#include "Hollow/Math/Matrix4.h"
#include "Hollow/Math/Quaternion.h"
#include "Hollow/Graphics/Vertex.h"

#include <fstream>
#include <unordered_map>
#include <string>

#define UNSIGNED_SHORT 5123
#define UNSIGNED_INT 5125
#define FLOAT 5126

namespace Hollow {
	struct NodeKeyFrameData
	{
		Vector3 vec3Val;
		Quaternion quatVal;
	};

	struct NodeAnimationData
	{
		int nodeId;
		std::unordered_map<float, NodeKeyFrameData> data;
	};

	struct AnimationNode
	{
		int gltfId;
		int id;
		Matrix4 localTransform;
		std::vector<AnimationNode*> childrens;
	};

	struct Animation
	{
		std::vector<NodeAnimationData> data;
	};

	struct Material
	{
		std::string diffuseTexture;
		std::string normalTexture;
		std::string specularTexture;
		std::string roughnesTexture;
		std::string emisiveTexture;
		std::string occlusionTexture;

		std::string name;
	};

	struct LoadedMesh
	{
		std::vector<Vector3> normals;
		std::vector<Vector3> positions;
		std::vector<Vector2> texCoords;
		std::vector<float*>  weigths;
		std::vector<unsigned short*> joints;
		std::vector<unsigned int> indices;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	struct Node
	{
		std::vector<Node*> childrens;
		Matrix4 transformation;
		std::string name;
		// index to mesh
		int mesh = -1;

		Node(std::string& name) :
			name(name)
		{}
	};

	struct GLTFModel
	{
		std::vector<Mesh*> meshes;
		Node* rootNode;
	};

	struct Model
	{
		std::vector<LoadedMesh> meshes;
		std::unordered_map<std::string, Material> materials;
		std::vector<Animation> animations;
		Node* rootNode;
		AnimationNode* animationRootNode;
		std::vector<AnimationNode*> animationNodes;
	};
}

class GLTFImporter
{
public:
	Hollow::GLTFModel* import(const char* filename);

	bool hasMesh(tinygltf::Node node, tinygltf::Model& model);
	void processMesh(Hollow::Node* node, tinygltf::Node childModelNode, Hollow::Model& model, tinygltf::Model& tModel, std::ifstream& file);
	void load(Hollow::Node* node, const tinygltf::Node& modelNode, tinygltf::Model& tModel, Hollow::Model& model, std::ifstream& file);
	void processAnimation(Hollow::Model& lModel, tinygltf::Model& model, std::ifstream& file);
	void processAnimationNode(Hollow::AnimationNode* node, const tinygltf::Node& modelNode, Hollow::Model& lModel, tinygltf::Model& model);
	void processSkin(Hollow::Model& lModel, tinygltf::Model& model, std::ifstream& file);
};