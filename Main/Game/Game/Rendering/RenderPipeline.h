#pragma once

#include "FrameBuffer.h"
#include "ECS/Systems/MeshRendererSystem.h"
#include "ECS/Systems/SkinnedMeshRendererSystem.h"
#include "ECS/Systems/CubeRenderSystem.h"
#include "ECS/Systems/BoxColliderRenderSystem.h"
#include "ECS/Systems/CircleColliderRenderSystem.h"

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
		std::shared_ptr<SkinnedMeshRendererSystem> skinnedMeshRender;
#ifdef _DEBUG
		std::shared_ptr<CubeRenderSystem> cubeRenderer;
		std::shared_ptr<BoxColliderRenderSystem> boxColliderRenderer;
		std::shared_ptr<CircleColliderRenderSystem> circleColliderRenderer;
#endif
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

