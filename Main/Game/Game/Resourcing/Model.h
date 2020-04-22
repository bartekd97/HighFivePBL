#pragma once

#include "ModelManager.h"

class Model {
	friend void ModelManager::Initialize();
	friend class ModelLibrary;

	typedef std::unordered_map<std::string, std::shared_ptr<Animation>> Animations;

public:
	const std::shared_ptr<Mesh> mesh;
	const std::shared_ptr<Material> material;
	const std::shared_ptr<SkinningData> skinningData;
	const Animations animations;

private:
	Model(
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<Material> material) :
		mesh(mesh), material(material) {}

	Model(
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<Material> material,
		std::shared_ptr<SkinningData> skinningData,
		Animations& animations) :
		mesh(mesh), material(material), skinningData(skinningData), animations(animations) {}
};