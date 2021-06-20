#pragma once

#include <glm/glm.hpp>
#include "Resourcing/Shader.h"
#include "ECS/System.h"

class GrassSimulatorSystem : public System, public ISystemUpdate, public ISystemPostUpdate
{
private:
	std::shared_ptr<Shader> circlePrimitiveShader;
	std::shared_ptr<Shader> simulatorShader;

public:
	void Init() override;
	void Update(float dt) override;
	void PostUpdate(float dt) override;
};