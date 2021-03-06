#pragma once

#include "Mesh.h"
#include "vendor/tinygltf/json.hpp"
#include "Hollow/Common/FileSystem.h"
#include "Hollow/Common/Helper.h"
#include "Hollow/Platform.h"

namespace Hollow {

	class HollowModelImporter
	{
	public:
		s_ptr<Import::Model> import(const char* filename)
		{
			using json = nlohmann::json;

			auto j = json::parse(FileSystem::getFileContent(filename));

			s_ptr<Import::Model> model(new Import::Model());

			std::string binFilename = j["Data"]["filename"].get<std::string>();

			std::string fileFolder = Helper::trimToLastLineEntry(filename, '/');
			std::string folder;

			if (fileFolder.size()) {
				folder = fileFolder;
			}
			std::ifstream file(binFilename, std::ios::binary);

			model->rootNode = j["RootNode"].get<int>();
			if (j.find("Skin") != j.end()) {
				model->skinned = true;
				model->rootJoint = j["Skin"]["rootJoint"].get<int>();
				model->joints = j["Skin"]["joints"].get<std::vector<int>>();
				Matrix4* matrixData = new Matrix4[model->joints.size()];
				file.seekg(j["Skin"]["inverseBindMatrices"].get<size_t>(), std::fstream::beg);
				file.read((char*)matrixData, sizeof(Matrix4) * model->joints.size());
				model->inverseBindMatrices = std::vector<Matrix4>(matrixData, matrixData + model->joints.size());
			}

			bool isopen = file.is_open();
			for (int i = 0; i < j["Meshes"].size(); i++) {
				s_ptr<Import::Mesh> mesh = std::make_shared<Import::Mesh>();
				mesh->material = j["Meshes"][i]["material"].get<int>();

				file.seekg(j["Meshes"][i]["vertices_offset"].get<size_t>(), std::fstream::beg);
				size_t verticesCount = j["Meshes"][i]["vertices_size"].get<size_t>();
				Vertex* vertices = new Vertex[verticesCount];
				file.read((char*)vertices, sizeof(Vertex) * verticesCount);

				mesh->vertices = std::vector<Vertex>(vertices, vertices + verticesCount);

				file.seekg(j["Meshes"][i]["indices_offset"].get<size_t>(), std::fstream::beg);
				unsigned int* indices = new unsigned int[j["Meshes"][i]["indices_size"].get<size_t>()];
				file.read((char*)indices, sizeof(unsigned int) * j["Meshes"][i]["indices_size"].get<int>());

				mesh->indices = std::vector<unsigned int>(indices, indices + j["Meshes"][i]["indices_size"].get<int>());
				model->meshes.push_back(mesh);

				delete[] vertices;
				delete[] indices;
			}

			for (int i = 0; i < j["Nodes"].size(); i++) {
				s_ptr<Import::Node> node = std::make_shared<Import::Node>();
				node->id = i;
				node->name = j["Nodes"][i]["name"].get<std::string>();
				node->meshId = j["Nodes"][i]["mesh"].get<int>();
				node->jointId = j["Nodes"][i]["joint"].get<int>();
				node->rotation = Quaternion(j["Nodes"][i]["rotation"][0].get<float>(), j["Nodes"][i]["rotation"][1].get<float>(), j["Nodes"][i]["rotation"][2].get<float>(), j["Nodes"][i]["rotation"][3].get<float>());
				node->translation = Vector3(j["Nodes"][i]["translation"][0].get<float>(), j["Nodes"][i]["translation"][1].get<float>(), j["Nodes"][i]["translation"][2].get<float>());
				node->scale = Vector3(j["Nodes"][i]["scale"][0].get<float>(), j["Nodes"][i]["scale"][1].get<float>(), j["Nodes"][i]["scale"][2].get<float>());
				node->childs = j["Nodes"][i]["childs"].get<std::vector<int>>();

				model->nodes.push_back(node);
			}

			for (int i = 0; i < j["Animations"].size(); i++) {
				s_ptr<Import::Animation> animation = std::make_shared<Import::Animation>();
				animation->name = j["Animations"][i]["name"].get<std::string>();
				animation->duration = j["Animations"][i]["duration"].get<double>();

				for (auto& it : j["Animations"][i]["data"]) {
					s_ptr<Import::AnimationData> animationData = std::make_shared<Import::AnimationData>();
					const int nodeId = it["node"].get<int>();

					// Handling rotations
					{
						const int size = it["rotations_size"].get<int>();

						if (size > 0) {
							double* times = new double[size];
							file.seekg(it["rotations_time_pos"].get<int>(), std::fstream::beg);
							file.read((char*)times, sizeof(double) * size);

							Quaternion* values = new Quaternion[size];
							file.seekg(it["rotations_value_pos"].get<int>(), std::fstream::beg);
							file.read((char*)values, sizeof(float) * 4 * size);

							for (int i = 0; i < size; i++) {
								animationData->rotations[times[i]] = values[i];
							}

							delete[] times;
							delete[] values;
						}
					}

					// Handling scale
					{
						const int size = it["scale_size"].get<int>();

						if (size > 0) {
							double* times = new double[size];
							file.seekg(it["scale_time_pos"].get<int>(), std::fstream::beg);
							file.read((char*)times, sizeof(double) * size);

							Vector3* values = new Vector3[size];
							file.seekg(it["scale_value_pos"].get<int>(), std::fstream::beg);
							file.read((char*)values, sizeof(float) * 3 * size);

							for (int i = 0; i < size; i++) {
								animationData->scale[times[i]] = values[i];
							}

							delete[] times;
							delete[] values;
						}
					}

					// Handling translations
					{
						const int size = it["translation_size"].get<int>();

						if (size > 0) {
							double* times = new double[size];
							file.seekg(it["translation_time_pos"].get<int>(), std::fstream::beg);
							file.read((char*)times, sizeof(double) * size);

							Vector3* values = new Vector3[size];
							file.seekg(it["translation_value_pos"].get<int>(), std::fstream::beg);
							file.read((char*)values, sizeof(float) * 3 * size);

							for (int i = 0; i < size; i++) {
								animationData->positions[times[i]] = values[i];
							}

							delete[] times;
							delete[] values;
						}
					}

					animation->data[nodeId] = animationData;
				}
				model->animations.push_back(animation);
			}

			file.close();

			for (int i = 0; i < j["Materials"].size(); i++) {
				Import::Material material;
				material.name = j["Materials"][i]["name"].get<std::string>();
				material.baseColorFactor = Hollow::Vector4(
					j["Materials"][i]["base_color"][0].get<float>(),
					j["Materials"][i]["base_color"][1].get<float>(),
					j["Materials"][i]["base_color"][2].get<float>(),
					j["Materials"][i]["base_color"][3].get<float>()
				);

				material.metallicFactor = j["Materials"][i]["metallic"].get<float>();
				material.roughnessFactor = j["Materials"][i]["roughness"].get<float>();
				//material.emissiveFactor = j["Materials"][i]["emissive"].get<float>();

				material.diffuseTexture = j["Materials"][i]["diffuse_texture"].get<std::string>();
				material.emisiveTexture = j["Materials"][i]["emissive_texture"].get<std::string>();
				material.normalTexture = j["Materials"][i]["normal_texture"].get<std::string>();
				material.occlusionTexture = j["Materials"][i]["occlusion_texture"].get<std::string>();
				material.roughnesTexture = j["Materials"][i]["roughness_texture"].get<std::string>();
				material.specularTexture = j["Materials"][i]["specular_texture"].get<std::string>();

				model->materials.push_back(material);
			}

			return model;
		}
	};

}