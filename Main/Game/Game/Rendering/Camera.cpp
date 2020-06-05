#include "Camera.h"
#include "Frustum.h"
#include "WindowManager.h"

Camera::Camera()
{
	position = glm::vec3(0, 0, 1);
	target = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);

	scale = 10.0f;
	fov = 45.0f;
	width = 1280;
	height = 720;
	nearPlane = 0.3f;
	farPlane = 250.0f;

	mode = PERSPECTIVE;

	dirty = true;
}


Camera::~Camera()
{
}

glm::vec3 Camera::GetPosition()
{
	return position;
}

glm::vec3 Camera::GetViewDiorection()
{
	return glm::normalize(target - position);
}

void Camera::SetView(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	this->position = position;
	this->target = target;
	this->up = up;
	dirty = true;
}

void Camera::SetView(glm::vec3 position, glm::vec3 target)
{
	glm::vec3 direction = glm::normalize(position - target);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(up, direction));
	glm::vec3 cameraUp = glm::cross(direction, right);
	SetView(position, target, cameraUp);
}

void Camera::SetView(Transform & transform)
{
	SetView(
		transform.GetWorldPosition(),
		transform.GetWorldPosition() + transform.GetWorldFront(),
		transform.GetWorldUp()
	);
}

glm::mat4 Camera::GetViewMatrix()
{
	return view;
}
glm::mat4 Camera::GetProjectionMatrix()
{
	return projection;
}
glm::mat4 Camera::GetInvViewMatrix()
{
	return invView;
}
glm::mat4 Camera::GetInvProjectionMatrix()
{
	return invProjection;
}


Frustum Camera::GetFrustum()
{
	if (dirty)
	{
		Update();
	}
	return Frustum(projection * view);
}



void Camera::SetMode(Mode mode)
{
	this->mode = mode;
	dirty = true;
}

void Camera::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;
	dirty = true;
}

void Camera::SetSize(glm::vec2 size)
{
	this->width = size.x;
	this->height = size.y;
	dirty = true;
}

void Camera::SetClipPlane(float nearPlane, float farPlane)
{
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	dirty = true;
}

void Camera::SetClipPlane(glm::vec2 planes)
{
	this->nearPlane = planes.x;
	this->farPlane = planes.y;
	dirty = true;
}

void Camera::SetFOV(float fov)
{
	this->fov = fov;
	dirty = true;
}
void Camera::SetScale(float scale)
{
	this->scale = scale;
	dirty = true;
}

Camera::Mode Camera::GetMode()
{
	return this->mode;
}

glm::vec2 Camera::GetSize()
{
	return { this->width, this->height };
}

glm::vec2 Camera::GetClipPlane()
{
	return { this->nearPlane, this->farPlane };
}

float Camera::GetFOV()
{
	return this->fov;
}

float Camera::GetScale()
{
	return this->scale;
}

glm::vec3 Camera::ScreenToWorldPosition(glm::vec2 position, float depth)
{
	if (dirty)
	{
		Update();
	}

	position /= glm::vec2(WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
	position -= glm::vec2(0.5f, 0.5f);
	position *= glm::vec2(2.0f, -2.0f);;
	float z = ((depth / (farPlane - nearPlane)) * 2.0f) - 1.0f;
	glm::vec4 world = invView * (invProjection * glm::vec4(position, z, 1.0f));

	return glm::vec3(world);
}




void Camera::Use(std::shared_ptr<Shader> shader)
{
	if (dirty)
	{
		Update();
	}

	shader->setMat4("gProjection", projection);
	shader->setMat4("gView", view);
	shader->setMat4("gInvProjection", invProjection);
	shader->setMat4("gInvView", invView);
	shader->setVector3F("gCameraPosition", position);
}

void Camera::Update()
{
	if (mode == PERSPECTIVE)
	{
		projection = glm::perspective(
			glm::radians(fov),
			width / height,
			nearPlane, farPlane);
	}
	else if (mode == ORTHOGRAPHIC)
	{
		projection = glm::ortho(
		(-width / 2.0f) * scale, (width / 2.0f) * scale,
			(-height / 2.0f) * scale, (height / 2.0f) * scale,
			nearPlane, farPlane);
	}

	view = glm::lookAt(position, target, up);

	invView = glm::inverse(view);
	invProjection = glm::inverse(projection);

	dirty = false;
}
