#pragma once

#include "FrameBuffer.h"
#include "ECS/Systems/MeshRendererSystem.h"
#include "ECS/Systems/CubeRenderSystem.h"

class RenderPipeline
{
public:
	enum class GBufferBindingPoint {
		POSITION = 1,
		NORMAL,
		ALBEDO,
		METALNESS_ROUGHNESS,
		EMISSIVE
	};
	struct GBufferStruct {
		std::shared_ptr<Texture> position;
		std::shared_ptr<Texture> normal;
		std::shared_ptr<Texture> albedo;
		std::shared_ptr<Texture> metalnessRoughness;
		std::shared_ptr<Texture> emissive;

		std::shared_ptr<FrameBuffer> frameBuffer;
	};
	struct RenderSystemsStruct {
		std::shared_ptr<MeshRendererSystem> meshRenderer;
		std::shared_ptr<CubeRenderSystem> cubeRenderer;
	};

private:
	GBufferStruct GBuffer;
	RenderSystemsStruct RenderSystems;

	bool initialized = false;
	std::shared_ptr<Shader> combineGBufferShader;

	void InitGBuffer();
	void InitRenderSystems();

public:
	void Init();
	void Render();
};

