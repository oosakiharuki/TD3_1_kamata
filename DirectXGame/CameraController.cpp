#include "CameraController.h"
#include "Mymath.h"
#include <KamataEngine.h>

#include "Player.h"

CameraController::CameraController() : offset_{0.0f, 5.0f, -20.0f}, pitchDeg_(10.0f) {}

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

	// 回転角をラジアンに変換
	float pitchRad = pitchDeg_ * (3.14159265f / 180.0f);
	float yawRad = yawDeg_ * (3.14159265f / 180.0f);

	// プレイヤーからの基本距離（cameraTranslate.zで調整可能）
	float distance = 20.0f + cameraTranslate.z;

	// ピッチ角に基づく水平距離を計算
	float horizontalDistance = distance * std::cos(pitchRad);

	// ピッチ角に基づく垂直オフセットを計算
	float verticalOffset = distance * std::sin(pitchRad);

	// プレイヤーを中心としたカメラ位置を計算
	camera->translation_.x = playerPosition.x - horizontalDistance * std::sin(yawRad);
	camera->translation_.y = playerPosition.y + 5.0f + cameraTranslate.y + verticalOffset;
	camera->translation_.z = playerPosition.z - horizontalDistance * std::cos(yawRad);

	// プレイヤーを見るようにカメラの回転を設定
	camera->rotation_.x = pitchRad;
	camera->rotation_.y = yawRad;
	camera->rotation_.z = 0.0f;

	// カメラ行列の更新と転送
	camera->UpdateViewMatrix();
	camera->TransferMatrix();
}