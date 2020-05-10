#pragma once

#include "FrameBuffer.h"

class Shader;

class MeshRendererSystem;
class SkinnedMeshRendererSystem;
class CubeRenderSystem;
class BoxColliderRenderSystem;
class CircleColliderRenderSystem;

class RenderPipeline
{
public:
	enum class GBufferBindingPoint {
		POSITION = 1,
		NORMAL,
		ALBEDO,
		METALNESS_ROUGHNESS_SHADOW,
		EMISSIVE
	};
	struct GBufferStruct {
		std::shared_ptr<Texture> position;
		std::shared_ptr<Texture> normal;
		std::shared_ptr<Texture> albedo;
		std::shared_ptr<Texture> metalnessRoughnessShadow;
		std::shared_ptr<Texture> emissive;

		std::shared_ptr<FrameBuffer> frameBuffer;
	};
	struct ShadowmapStruct {
		std::shared_ptr<Texture> depthmap;

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
	ShadowmapStruct Shadowmap;
	RenderSystemsStruct RenderSystems;

	bool initialized = false;
	std::shared_ptr<Shader> combineGBufferShader;

	void InitGBuffer();
	void InitShadowmap();
	void InitRenderSystems();

public:
	void Init();
	void Render();
};

