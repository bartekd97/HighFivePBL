#pragma once

#include "ModelManager.h"

class Model {
	friend void ModelManager::Initialize();
	friend class ModelLibrary;

public:
	const std::shared_ptr<Mesh> mesh;
	const std::shared_ptr<Material> material;
	const std::shared_ptr<SkinningData> skinningData;

private:
	Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, std::shared_ptr<SkinningData> skinningData) :
		mesh(mesh), material(material), skinningData(skinningData) {}
};