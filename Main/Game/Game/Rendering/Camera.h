#pragma once

#include "ECS/Components/Transform.h"
#include "Resourcing/Shader.h"

class Frustum;
class Camera
{
public:
	enum Mode {
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;

	float scale;
	float fov;
	float width;
	float height;
	float nearPlane;
	float farPlane;

	Mode mode;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 invView;
	glm::mat4 invProjection;

	bool dirty;
public:
	Camera();
	~Camera();

	glm::vec3 GetPosition();
	glm::vec3 GetViewDiorection();

	void SetView(glm::vec3 position, glm::vec3 target, glm::vec3 up);
	void SetView(glm::vec3 position, glm::vec3 target);
	void SetView(Transform& transform);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetInvViewMatrix();
	glm::mat4 GetInvProjectionMatrix();

	Frustum GetFrustum();

	void SetMode(Mode mode);
	void SetSize(float width, float height);
	void SetSize(glm::vec2 size);
	void SetClipPlane(float nearPlane, float farPlane);
	void SetClipPlane(glm::vec2 planes);
	// FOV only for perspective
	void SetFOV(float fov);
	// scale only for orthographic
	void SetScale(float scale);

	Mode GetMode();
	glm::vec2 GetSize();
	glm::vec2 GetClipPlane();
	float GetFOV();
	float GetScale();

	void Use(std::shared_ptr<Shader> shader);

private:
	void Update();
};

