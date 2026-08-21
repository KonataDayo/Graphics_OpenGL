#pragma once
#include <string>
#include <vector>

#include "Mesh.h"

namespace util
{
	class ObjParser
	{
	private:
		static void ParseFace(const std::string& token, const std::vector<glm::vec3>& positions, 
			const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& tex_coords,
			std::vector<Vertex>& vertices);

	public:
		ObjParser();
		~ObjParser();

		static bool ParseOBJ(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);


	};
}
