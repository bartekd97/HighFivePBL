#pragma once

#include "ModelManager.h"
#include "Mesh.h"

class Model {
	friend void ModelManager::Initialize();
	friend class ModelLibrary;

public:
	const std::shared_ptr<Mesh> mesh;
	const std::shared_ptr<Material> material;

private:
	Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
		mesh(mesh), material(material) {}
};