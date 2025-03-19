#include "CameraController.h"
#include "Mymath.h"
#include <KamataEngine.h>

#include "Player.h"

CameraController::CameraController() : offset_{0.0f, 20.0f, -25.0f}, pitchDeg_(35.0f) {}

CameraController::~CameraController() {}

void CameraController::SetOffset(const Vector3& offset) { offset_ = offset; }

void CameraController::SetPitch(float pitchDeg) { pitchDeg_ = pitchDeg; }

void CameraController::SetYaw(float yawDeg) { yawDeg_ = yawDeg; }

void CameraController::Update(Camera* camera, const Vector3& playerPosition) {
	if (camera == nullptr) {
		return;
	}
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("cameraTranslate", &camera->translation_.x);
	ImGui::DragFloat3("cameraRotate", &camera->rotation_.x);
	ImGui::DragFloat3("offset", &offset_.x);
	ImGui::End();
#endif

	// Convert rotation angles from degrees to radians
	float pitchRad = pitchDeg_ * (3.14159265f / 180.0f);
	float yawRad = yawDeg_ * (3.14159265f / 180.0f);

	// Base distance from player (adjustable with cameraTranslate.z)
	float distance = 20.0f - cameraTranslate.z;

	// Calculate horizontal distance based on pitch
	float horizontalDistance = distance * std::cos(pitchRad);

	// Calculate vertical offset based on pitch
	float verticalOffset = distance * std::sin(pitchRad);

	// Calculate camera position using player position as pivot
	camera->translation_.x = playerPosition.x - horizontalDistance * std::sin(yawRad);
	camera->translation_.y = playerPosition.y + 5.0f + cameraTranslate.y + verticalOffset;
	camera->translation_.z = playerPosition.z - horizontalDistance * std::cos(yawRad);

	// Set camera rotation to look at player
	camera->rotation_.x = pitchRad;
	camera->rotation_.y = yawRad;
	camera->rotation_.z = 0.0f;

	// Update camera matrices
	camera->UpdateViewMatrix();
	camera->TransferMatrix();
}