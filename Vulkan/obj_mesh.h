#pragma once
#include "config.h"

namespace vkMesh {
	enum class ColorID {
		R = 1,
		G,
		B
	};
	enum class PosID {
		X = 1,
		Y,
		Z
	};

	class ObjMesh {
	public:

		std::vector<float> vertices;
		std::vector<uint32_t> indices;
		std::unordered_map<std::string, uint32_t> history;
		std::unordered_map<std::string, glm::vec3> colors;
		glm::vec3 brushColor;

		std::vector<glm::vec3> v, vn;
		std::vector<glm::vec2> vt;
		glm::mat4 preTransform;

		ObjMesh(const char* objFilePatth, const char* mtlFilePath, glm::mat4 preTransform);
		void read_vertex_data(const std::vector<std::string>& words);
		void read_texcoord_data(const std::vector<std::string>& words);
		void read_normal_data(const std::vector<std::string>& words);
		void read_face_data(const std::vector<std::string>& words);
		void read_corner(const std::string& vertexDescription);
	};
}