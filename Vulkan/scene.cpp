#include "scene.h"

Scene::Scene() {
	positions.insert({ meshTypes::GROUND, {} });
	positions.insert({ meshTypes::GIRL, {} });

	positions[meshTypes::GROUND].push_back(glm::vec3(10.f, 0.0f, 0.0f));
	positions[meshTypes::GIRL].push_back(glm::vec3(17.0f, 0.0f, 0.0f));
}