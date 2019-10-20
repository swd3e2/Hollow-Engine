#pragma once

#include "Hollow/Common/Log.h"
#include "vendor/assimp/assimp/Importer.hpp"
#include "vendor/assimp/assimp/scene.h"     
#include "vendor/assimp/assimp/postprocess.h"
#include <vector>
#include "Hollow/Math/Vector4.h"
#include "Hollow/Math/Vector3.h"
#include "Hollow/Math/Vector2.h"
#include <unordered_map>
#include "Mesh.h"

namespace Hollow {
	class AssimpImporter
	{
	private:
		unsigned int animationNodeNextId = 0;
	public:
		AssimpImporter() {  }
		~AssimpImporter() {  }

		s_ptr<Import::Model> import(const char* filename, bool async = true)
		{
			animationNodeNextId = 0;

			s_ptr<Import::Model> model(new Import::Model());

			Assimp::Importer importer;

			const aiScene* scene = importer.ReadFile(filename,
				aiProcessPreset_TargetRealtime_Quality
			);

			if (!scene) 
				return model;

			std::unordered_map<std::string, Import::AnimationNode*> animationNodes;
			
			if (scene->mNumAnimations > 0) {
				model->rootNode = new Import::AnimationNode();
				model->rootNode->id = animationNodeNextId++;
				model->rootNode->name = scene->mRootNode->mName.C_Str();

				model->rootNode->localTransform = Matrix4::transpose(Matrix4(
					scene->mRootNode->mTransformation.a1, scene->mRootNode->mTransformation.a2, scene->mRootNode->mTransformation.a3, scene->mRootNode->mTransformation.a4,
					scene->mRootNode->mTransformation.b1, scene->mRootNode->mTransformation.b2, scene->mRootNode->mTransformation.b3, scene->mRootNode->mTransformation.b4,
					scene->mRootNode->mTransformation.c1, scene->mRootNode->mTransformation.c2, scene->mRootNode->mTransformation.c3, scene->mRootNode->mTransformation.c4,
					scene->mRootNode->mTransformation.d1, scene->mRootNode->mTransformation.d2, scene->mRootNode->mTransformation.d3, scene->mRootNode->mTransformation.d4
				));

				animationNodes[scene->mRootNode->mName.C_Str()] = model->rootNode;

				parseAnimationData(animationNodes, model, scene);
			}

			parseCommonData(animationNodes, model, scene);

			importer.FreeScene();

			return model;
		}
		private:
		// Mesh data
		void parseCommonData(std::unordered_map<std::string, Import::AnimationNode*>& animationNodes, const s_ptr<Import::Model>& data, const aiScene* scene)
		{
			if (scene->mNumMeshes > 0) {
				aiNode* temp;

				for (int i = 0; i < scene->mNumMeshes; i++) {
					aiMesh* aMesh = scene->mMeshes[i];
					Import::Mesh* mesh = new Import::Mesh();
					mesh->name = aMesh->mName.C_Str();
					mesh->material = aMesh->mMaterialIndex;

					for (int j = 0; j < aMesh->mNumVertices; j++) {
						Vertex vertex;
						aiVector3D& position = aMesh->mVertices[j];

						vertex.pos.x = position.x;
						vertex.pos.y = position.y;
						vertex.pos.z = position.z;

						// Find lowest and highest vertices for AABB culling
						if (vertex.pos.x < data->A.x && vertex.pos.y < data->A.y && vertex.pos.z < data->A.z) {
							data->A = vertex.pos;
						}
						if (vertex.pos.x > data->B.x&& vertex.pos.y > data->B.y&& vertex.pos.z > data->B.z) {
							data->B = vertex.pos;
						}

						if (scene->mMeshes[i]->HasNormals()) {
							aiVector3D& normal = aMesh->mNormals[j];

							vertex.normal.x = normal.x;
							vertex.normal.y = normal.y;
							vertex.normal.z = normal.z;
						}

						if (scene->mMeshes[i]->HasTextureCoords(0)) {
							vertex.texCoord.x = aMesh->mTextureCoords[0][j].x;
							vertex.texCoord.y = 1.0f - aMesh->mTextureCoords[0][j].y;
						}

						if (scene->mMeshes[i]->HasTangentsAndBitangents()) {
							vertex.tangent.x = aMesh->mTangents[j].x;
							vertex.tangent.y = aMesh->mTangents[j].y;
							vertex.tangent.z = aMesh->mTangents[j].z;
							vertex.bitangent.x = aMesh->mBitangents[j].x;
							vertex.bitangent.y = aMesh->mBitangents[j].y;
							vertex.bitangent.z = aMesh->mBitangents[j].z;
						}

						mesh->vertices.push_back(vertex);
					}

					/* Indices */
					int size = scene->mMeshes[i]->mNumFaces * aMesh->mFaces[0].mNumIndices;
					unsigned int* indices = new unsigned int[size];

					for (int j = 0; j < aMesh->mNumFaces; j++) {
						for (int k = 0; k < aMesh->mFaces[j].mNumIndices; k++) {
							mesh->indices.push_back(aMesh->mFaces[j].mIndices[k]);
						}
					}

					if (data->materials.find(mesh->material) == data->materials.end()) {
						aiString str;

						// Material data
						aiMaterial* aMaterial = scene->mMaterials[aMesh->mMaterialIndex];

						Import::Material material;
						material.name = aMaterial->GetName().C_Str();

						aiColor3D color(0.f, 0.f, 0.f);
						aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
						material.baseColorFactor = Vector4(color.r, color.g, color.b, 1.0f);


						if (aiReturn_SUCCESS == aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &str)) {
							material.diffuseTexture = str.C_Str();
						}
						if (aiReturn_SUCCESS == aMaterial->GetTexture(aiTextureType_HEIGHT, 0, &str)) {
							material.normalTexture = str.C_Str();
						}
						if (aiReturn_SUCCESS == aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &str)) {
							material.specularTexture = str.C_Str();
						}

						data->materials[mesh->material] = material;
					}

					if (scene->mMeshes[i]->HasBones()){
						for (int j = 0; j < scene->mMeshes[i]->mNumBones; j++) {
							aiBone* aibone = scene->mMeshes[i]->mBones[j];

							if (animationNodes.find(aibone->mName.C_Str()) == animationNodes.end()){
								HW_ERROR("Animation node presented in mesh not found in nodes list, {}", aibone->mName.C_Str());
								continue;
							}

							Import::AnimationNode* node = animationNodes[aibone->mName.C_Str()];

							for (int k = 0; k < aibone->mNumWeights; k++) {
								int vertexId = aibone->mWeights[k].mVertexId;
								float weight = aibone->mWeights[k].mWeight;

								for (int l = 0; l < 4; l++) {
									if (mesh->vertices[vertexId].boneData.weights[l] == 0.0) {
										mesh->vertices[vertexId].boneData.joints[l] = node->id;
										mesh->vertices[vertexId].boneData.weights[l] = weight;
										break;
									}
								}
							}
						}
					}

					data->meshes.push_back(mesh);
				}
			}
		}

		void parseAnimationData(std::unordered_map<std::string, Import::AnimationNode*>& animationNodes, s_ptr<Import::Model>& data, const aiScene* scene)
		{
			// Create assimp node map 
			std::unordered_map<std::string, aiNode*> assimpNodes;
			createAssimpNodeMap(assimpNodes, scene->mRootNode);

			// Create list of bones that used as bones
			std::unordered_map<std::string, aiNode*> assimpBoneNodes;

			for (int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* assimpAnimation = scene->mAnimations[i];
				for (int j = 0; j < assimpAnimation->mNumChannels; j++) {
					aiNodeAnim* assimpChannel = assimpAnimation->mChannels[j];
					assimpBoneNodes[assimpChannel->mNodeName.C_Str()] = assimpNodes[assimpChannel->mNodeName.C_Str()];
				}
			}

			// Iterate up to root till find the node that isn't in bone list
			aiNode* rootNode = assimpBoneNodes.begin()->second;
			while (rootNode->mParent != nullptr && assimpBoneNodes.find(rootNode->mParent->mName.C_Str()) != assimpBoneNodes.end()) {
				rootNode = rootNode->mParent;
			}

			data->rootNode = new Import::AnimationNode();
			data->rootNode->id = animationNodeNextId++;
			data->rootNode->name = rootNode->mName.C_Str();

			data->rootNode->localTransform = Matrix4(
				rootNode->mTransformation.a1, rootNode->mTransformation.a2, rootNode->mTransformation.a3, rootNode->mTransformation.a4,
				rootNode->mTransformation.b1, rootNode->mTransformation.b2, rootNode->mTransformation.b3, rootNode->mTransformation.b4,
				rootNode->mTransformation.c1, rootNode->mTransformation.c2, rootNode->mTransformation.c3, rootNode->mTransformation.c4,
				rootNode->mTransformation.d1, rootNode->mTransformation.d2, rootNode->mTransformation.d3, rootNode->mTransformation.d4
			);

			animationNodes[rootNode->mName.C_Str()] = data->rootNode;
			createAnimationNodes(assimpBoneNodes, animationNodes, rootNode, data->rootNode);

			for (int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* assimpAnimation = scene->mAnimations[i];
				Import::Animation* animation = new Import::Animation();

				for (int j = 0; j < assimpAnimation->mNumChannels; j++) {
					aiNodeAnim* assimpChannel = assimpAnimation->mChannels[j];

					if (animationNodes.find(assimpChannel->mNodeName.C_Str()) == animationNodes.end()) {
						HW_ERROR("Could not find animation node in nodes list, {}", assimpChannel->mNodeName.C_Str());
						continue;
					}

					Import::AnimationNode* node = animationNodes[assimpChannel->mNodeName.C_Str()];
					assimpBoneNodes[assimpChannel->mNodeName.C_Str()] = assimpNodes[assimpChannel->mNodeName.C_Str()];
					Import::AnimationNodeData* animationData = nullptr;

					if (animation->data.find(node->id) != animation->data.end()) {
						animationData = animation->data[node->id];
					} else {
						animation->data[node->id] = animationData = new Import::AnimationNodeData;
					}

					for (int k = 0; k < assimpChannel->mNumPositionKeys; k++) {
						animationData->positions[assimpChannel->mPositionKeys[k].mTime] = Vector3(
							assimpChannel->mPositionKeys[k].mValue.x, 
							assimpChannel->mPositionKeys[k].mValue.y, 
							assimpChannel->mPositionKeys[k].mValue.z
						);
					}

					for (int k = 0; k < assimpChannel->mNumRotationKeys; k++) {
						animationData->rotations[assimpChannel->mRotationKeys[k].mTime] = Quaternion(
							assimpChannel->mRotationKeys[k].mValue.x,
							assimpChannel->mRotationKeys[k].mValue.y,
							assimpChannel->mRotationKeys[k].mValue.z, 
							assimpChannel->mRotationKeys[k].mValue.w
						);
					}

					for (int k = 0; k < assimpChannel->mNumScalingKeys; k++) {
						animationData->scale[assimpChannel->mScalingKeys[k].mTime] = Vector3(
							assimpChannel->mScalingKeys[k].mValue.x, 
							assimpChannel->mScalingKeys[k].mValue.y, 
							assimpChannel->mScalingKeys[k].mValue.z
						);
					}
				}
				data->animations.push_back(animation);
			}
		}

		void createAnimationNodes(std::unordered_map<std::string, aiNode*>& assimpBoneNodes, std::unordered_map<std::string, Import::AnimationNode*>& animationNodes, const aiNode* node, Import::AnimationNode* parentNode)
		{
			for (int i = 0; i < node->mNumChildren; i++) {
				if (assimpBoneNodes.find(node->mChildren[i]->mName.C_Str()) == assimpBoneNodes.end())
					continue;

				Import::AnimationNode* animNode = new Import::AnimationNode();
				animNode->id = animationNodeNextId++;
				animNode->name = node->mChildren[i]->mName.C_Str();

				animNode->localTransform = Matrix4::transpose(Matrix4(
					node->mChildren[i]->mTransformation.a1, node->mChildren[i]->mTransformation.a2, node->mChildren[i]->mTransformation.a3, node->mChildren[i]->mTransformation.a4,
					node->mChildren[i]->mTransformation.b1, node->mChildren[i]->mTransformation.b2, node->mChildren[i]->mTransformation.b3, node->mChildren[i]->mTransformation.b4,
					node->mChildren[i]->mTransformation.c1, node->mChildren[i]->mTransformation.c2, node->mChildren[i]->mTransformation.c3, node->mChildren[i]->mTransformation.c4,
					node->mChildren[i]->mTransformation.d1, node->mChildren[i]->mTransformation.d2, node->mChildren[i]->mTransformation.d3, node->mChildren[i]->mTransformation.d4
				));

				animationNodes[node->mChildren[i]->mName.C_Str()] = animNode;
				parentNode->childrens.push_back(animNode);

				createAnimationNodes(assimpBoneNodes, animationNodes, node->mChildren[i], animNode);
			}
		}

		void createAssimpNodeMap(std::unordered_map<std::string, aiNode*>& assimpNodes, const aiNode* node)
		{
			for (int i = 0; i < node->mNumChildren; i++) {
				assimpNodes[node->mChildren[i]->mName.C_Str()] = node->mChildren[i];
				createAssimpNodeMap(assimpNodes, node->mChildren[i]);
			}
		}
	};
}