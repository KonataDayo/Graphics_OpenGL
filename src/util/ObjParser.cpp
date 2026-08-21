#include "ObjParser.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "glm/detail/type_vec2.hpp"

void util::ObjParser::ParseFace(const std::string& token, const std::vector<glm::vec3>& positions,
                                const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& tex_coords,
								std::vector<Vertex>& vertices)
{
	unsigned int posIndex, texIndex, normIndex;
	sscanf_s(token.c_str(), "%d/%d/%d", &posIndex, &texIndex, &normIndex);
	posIndex--;
	texIndex--;
	normIndex--;
	Vertex vertex;
	vertex.Position = positions[posIndex];
	vertex.Normal = normals[normIndex];
	vertex.TexCoord = tex_coords[texIndex];
	vertices.push_back(vertex);
}

util::ObjParser::ObjParser()
{
}

util::ObjParser::~ObjParser()
{
}

bool util::ObjParser::ParseOBJ(const std::string& filepath, std::vector<Vertex>& vertices,
	std::vector<unsigned int>& indices)
{
	std::ifstream fd(filepath);
	if (!fd.is_open())
	{
		std::cout << " Error! [ObjParser] (ParseOBJ) failed to open file!" << std::endl;
		return false;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;

	std::string line; 
	while (std::getline(fd, line))
	{
		std::stringstream ss(line);

		std::string type;
		ss >> type;

		// vertex
		if (type == "v") 
		{
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}

		// normal
		else if (type == "vn")
		{
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}

		// texture coordinate
		else if (type == "vt")
		{
			glm::vec2 tex_coord;
			ss >> tex_coord.x >> tex_coord.y;
			tex_coords.push_back(tex_coord);
			// TODO: texture coordinate
		}

		// face
		else if (type == "f")
		{
			std::string v1, v2 ,v3;
			ss >> v1 >> v2 >> v3;
			ParseFace(v1,positions, normals, tex_coords, vertices);
			ParseFace(v2, positions, normals, tex_coords, vertices);
			ParseFace(v3, positions, normals, tex_coords, vertices);
			unsigned int start_index = vertices.size() - 3;
			indices.push_back(start_index);
			indices.push_back(start_index + 1);
			indices.push_back(start_index + 2);
		}
	}
	return true;
}
