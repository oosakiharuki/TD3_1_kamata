#include "CameraController.h"
#include <KamataEngine.h>
#include "Mymath.h"

#include "Player.h"

CameraController::CameraController() : offset_{0.0f, 3.0f, -20.0f}, pitchDeg_(0.0f) {}

//offset_{0.0f, 3.0f, -20.0f}, pitchDeg_(360.0f) {}　普通
//offset_{0.0f, 20.0f, -0.0f}, pitchDeg_(85.0f), yawDeg_(0.0f) {}　真上

CameraController::~CameraController() {}

void CameraController::SetOffset(const Vector3& offset) { offset_ = offset; }

void CameraController::SetPitch(float pitchDeg) { pitchDeg_ = pitchDeg; }

void CameraController::SetYaw(float yawDeg) { yawDeg_ = yawDeg; }

void CameraController::Update(Camera* camera, const Vector3& playerPosition) {
	if (camera == nullptr) {
		return;
	}

	ImGui::Begin("camera");
	ImGui::DragFloat3("cameraTranslate", &camera->translation_.x);
	ImGui::DragFloat3("cameraRotate", &camera->rotation_.x);
	ImGui::DragFloat3("offset", &offset_.x);
	ImGui::End();
	
	//カメラ開店に合わせて回転させる
	offset_ = {0.0f, 3.0f, -20.0f};

	Matrix4x4 rotate = camera->matView;

	offset_ = TransformNormal(offset_, rotate);

	// プレイヤーの座標にオフセットを加えてカメラ位置を設定
	camera->translation_.x = playerPosition.x - offset_.x;
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
