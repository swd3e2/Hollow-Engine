#include "ObjParser.h"

namespace Hollow {

	MeshData * ObjParser::LoadObj(const char * filename, const char * material_base_dir)
	{
		MeshData* data = new MeshData();

		std::ifstream filestream(filename);

		if (!filestream)
			Hollow::Log::GetCoreLogger()->error("ObjParser: can't open filestream, filename {}", filename);

		std::string linebuff;
		int lines = 0;

		while (filestream.peek() != -1)
		{
			safeGetline(filestream, linebuff);
			lines++;

			// Trim newline '\r\n' or '\n'
			if (linebuff.size() > 0) {
				if (linebuff[linebuff.size() - 1] == '\n')
					linebuff.erase(linebuff.size() - 1);
			}
			if (linebuff.size() > 0) {
				if (linebuff[linebuff.size() - 1] == '\r')
					linebuff.erase(linebuff.size() - 1);
			}
			// Skip if empty line.
			if (linebuff.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = linebuff.c_str();
			token += strspn(token, " \t");

			if (token[0] == '#' || token[0] == '\0') continue;

			if (token[0] == 'v' && token[1] == ' ') {
				token += 2;
				parseVertices(data, token);
			}
			if (token[0] == 'v' && token[1] == 'n' && token[2] == ' ') {
				token += 3;
				parseNormals(data, token);
			}
			if (token[0] == 'v' && token[1] == 't' && token[2] == ' ') {
				token += 3;
				parseTexCoords(data, token);
			}
			if (token[0] == 'f' && token[1] == ' ') {
				token += 2;
				parseFace(currentObject, token);
			}
			if (token[0] == 'm' && token[1] == 't' && token[2] == 'l' && token[3] == 'l' && token[4] == 'i' && token[5] == 'b') {
				token += 7;
				std::string base_dir = material_base_dir;
				base_dir += token;
				LoadMtl(base_dir);
			}
			if (token[0] == 'o' && token[1] == ' ') {
				token += 2;
				if (currentObject != nullptr)
					data->objects.push_back(currentObject);

				currentObject = new RawMeshData();
				currentObject->object_name = token;
			}
		}
		if (currentObject != nullptr)
			data->objects.push_back(currentObject);

		Hollow::Log::GetCoreLogger()->warn("objects {}", data->objects.size());

		return data;
	}

	void ObjParser::parseVertices(MeshData * data, const char * token)
	{
		char* nextToken;
		data->vertices.push_back(atof(strtok_s((char*)token, " \t\r", &nextToken)));
		data->vertices.push_back(atof(strtok_s(nextToken, " \t\r", &nextToken)));
		data->vertices.push_back(atof(strtok_s(nextToken, " \t\r", &nextToken)));
	}

	void ObjParser::parseTexCoords(MeshData * data, const char * token)
	{
		char* nextToken;
		data->tex_coords.push_back(atof(strtok_s((char*)token, " \t\r", &nextToken)));
		data->tex_coords.push_back(atof(strtok_s(nextToken, " \t\r", &nextToken)));
	}

	void ObjParser::parseNormals(MeshData * data, const char * token) 
	{
		char* nextToken;
		data->normals.push_back(atof(strtok_s((char*)token, " \t\r", &nextToken)));
		data->normals.push_back(atof(strtok_s(nextToken, " \t\r", &nextToken)));
		data->normals.push_back(atof(strtok_s(nextToken, " \t\r", &nextToken)));
	}

	void ObjParser::parseFace(RawMeshData * mesh, const char * token)
	{
		char* nextFace;
		char* temp_str;
		char* temp_face;

		while ((temp_face = strtok_s((char*)token, WHITESPACE, (char**)&token)) != NULL) {
			Face face;
			face.vertice_index = atoi(temp_face) - 1;

			if (contains(temp_face, "//")) {
				temp_str = (char*)strchr(temp_face, '/');
				temp_str++;
				face.normal_index = atoi(++temp_str) - 1;
			}
			else if (contains(temp_face, "/")) {
				temp_str = (char*)strchr(temp_face, '/');
				face.tex_coord_index = atoi(++temp_str) - 1;

				if (contains(temp_str, "/"))
				{
					temp_str = strchr(temp_str, '/');
					face.normal_index = atoi(++temp_str) - 1;
				}
			}
			mesh->indices.push_back(face);
		}
	}

	void ObjParser::LoadMtl(std::string base_dir)
	{
		std::fstream filestream(base_dir);
		if (!filestream)
			Hollow::Log::GetCoreLogger()->error("ObjParser: can't open filestream, filename {}", base_dir);


		Material* current_material;

		std::string linebuff;
		int lines = 0;

		while (filestream.peek() != -1)
		{
			safeGetline(filestream, linebuff);
			lines++;

			// Trim newline '\r\n' or '\n'
			if (linebuff.size() > 0) {
				if (linebuff[linebuff.size() - 1] == '\n')
					linebuff.erase(linebuff.size() - 1);
			}
			if (linebuff.size() > 0) {
				if (linebuff[linebuff.size() - 1] == '\r')
					linebuff.erase(linebuff.size() - 1);
			}
			// Skip if empty line.
			if (linebuff.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = linebuff.c_str();
			token += strspn(token, " \t");


		}

		filestream.close();
	}

}