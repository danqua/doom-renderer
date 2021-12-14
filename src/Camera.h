#pragma once
#include <glm/glm.hpp>

struct Camera
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
};

void CreateCamera(Camera* camera, float fov, float aspect, float near, float far);
void CameraMoveForward(Camera* camera, float speed);
void CameraMoveRight(Camera* camera, float speed);
void CameraMoveUp(Camera* camera, float speed);
void CameraRotateOnAxis(Camera* camera, const glm::vec3& axis, float amount);