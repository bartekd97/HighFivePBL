#pragma once

#include "FrameBuffer.h"

class Shader;

class MeshRendererSystem;
class SkinnedMeshRendererSystem;
class CubeRenderSystem;
class BoxColliderRenderSystem;
class CircleColliderRenderSystem;

class IPostprocessingEffect;

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

		std::shared_ptr<Texture> depth;

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
	std::shared_ptr<FrameBuffer> PostprocessingSwapBuffers[2];
	ShadowmapStruct Shadowmap;
	RenderSystemsStruct RenderSystems;

	bool initialized = false;
	std::shared_ptr<Shader> combineGBufferShader;

	std::vector<std::shared_ptr<IPostprocessingEffect>> postprocessingEffects;

	void InitGBuffer();
	void InitShadowmap();
	void InitRenderSystems();
	void InitPostprocessingEffects();

public:
	void Init();
	void Render();

	inline const GBufferStruct& const GetGBuffer() { return GBuffer; }
};

