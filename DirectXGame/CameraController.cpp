#include "CameraController.h"
#include <KamataEngine.h>

CameraController::CameraController() : offset_{0.0f,20.0f, -0.0f}, pitchDeg_(85.0f), yawDeg_(0.0f) {}

CameraController::~CameraController() {}

void CameraController::SetOffset(const Vector3& offset) { offset_ = offset; }

void CameraController::SetPitch(float pitchDeg) { pitchDeg_ = pitchDeg; }

void CameraController::Update(Camera* camera, const Vector3& playerPosition) {
	if (camera == nullptr) {
		return;
	}

	// QとEキーの入力処理
	if (Input::GetInstance()->PushKey(DIK_Q)) {
		yawDeg_ -= 1.0f; // Qキーで左回転
	}
	if (Input::GetInstance()->PushKey(DIK_E)) {
		yawDeg_ += 1.0f; // Eキーで右回転
	}

	// プレイヤーの座標にオフセットを加えてカメラ位置を設定
	camera->translation_.x = playerPosition.x + offset_.x;
	camera->translation_.y = playerPosition.y + offset_.y;
	camera->translation_.z = playerPosition.z + offset_.z;

	// カメラの回転を設定
	camera->rotation_.x = pitchDeg_ * (3.14159265f / 180.0f);
	camera->rotation_.y = yawDeg_ * (3.14159265f / 180.0f);
	camera->rotation_.z = 0.0f;

	// カメラ行列の更新と転送
	camera->UpdateViewMatrix();
	camera->TransferMatrix();
}
