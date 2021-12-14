#include "Camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::GetForwardVector() const
{
	return glm::quat(rotation) * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::GetRightVector() const
{
	return glm::quat(rotation) * glm::vec3(1.0f, 0.0f, 0.0f);
}

void UpdateViewMatrix(Camera* camera)
{
	glm::vec3 forward = camera->GetForwardVector();
	camera->view = glm::lookAt(camera->position, camera->position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

void CreateCamera(Camera* camera, float fov, float aspect, float near, float far)
{
	camera->projection = glm::perspective(glm::radians(fov), aspect, near, far);
	UpdateViewMatrix(camera);
}

void CameraMoveForward(Camera* camera, float speed)
{
	camera->position += camera->GetForwardVector() * speed;
	UpdateViewMatrix(camera);
}

void CameraMoveRight(Camera* camera, float speed)
{
	camera->position += camera->GetRightVector() * speed;
	UpdateViewMatrix(camera);
}

void CameraRotateOnAxis(Camera* camera, const glm::vec3& axis, float amount)
{
	camera->rotation += axis * amount;
	UpdateViewMatrix(camera);
}

void CameraMoveUp(Camera* camera, float speed)
{
	camera->position.y += speed;
	UpdateViewMatrix(camera);
}