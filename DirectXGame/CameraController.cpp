#include "CameraController.h"
#include "Player.h"
#include <cmath>

// 度からラジアンに変換する簡易関数
inline float ToRadians(float degrees) { return degrees * 3.14159265f / 180.0f; }

CameraController::CameraController() {}
CameraController::~CameraController() {}

void CameraController::Initialize() {
	// カメラの初期化（ビュー行列・射影行列など）
	camera_.Initialize();

	// 好みでカメラのプロジェクション(パースペクティブ設定)を変更
	// 例）FOV=45°, アスペクト比=16:9, ニア=0.1f, ファー=1000.0f
	camera_.SetPerspective(ToRadians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);

	// 初期ビュー行列更新
	camera_.UpdateViewMatrix();
	camera_.TransferMatrix();
}

void CameraController::Update() {
	if (!target_) {
		return;
	}

	// 1) プレイヤーのワールド座標を取得
	const WorldTransform& wt = target_->GetWorldTransform();
	Vector3 playerPos = wt.translation_;

	// 2) カメラの目標位置を算出
	//    - Player の後ろ distance_ 分 + 上に height_ 分 + 任意のオフセット
	//    - rotateAngleX_ (度) で見下ろす
	float offsetY = height_;
	float offsetZ = -distance_; // プレイヤーの背面側(奥方向)

	// 現在のカメラ位置（カメラ_.translation_）を使い、目標の座標を算出
	//   ※ここでは簡単に (playerPos + {0, offsetY, offsetZ}) としています
	//   ※もし Player の向きを考慮したい場合は、回転角度を反映した上で offsetZ を決定します
	Vector3 desiredPos;
	desiredPos.x = playerPos.x + targetOffset_.x;
	desiredPos.y = playerPos.y + targetOffset_.y + offsetY;
	desiredPos.z = playerPos.z + targetOffset_.z + offsetZ;

	// 3) カメラ現在位置と目標位置を線形補間 (Lerp) でゆったり移動
	Vector3 currentPos = camera_.translation_;
	Vector3 newPos;
	newPos.x = currentPos.x + (desiredPos.x - currentPos.x) * lerpRate_;
	newPos.y = currentPos.y + (desiredPos.y - currentPos.y) * lerpRate_;
	newPos.z = currentPos.z + (desiredPos.z - currentPos.z) * lerpRate_;

	// 4) カメラの回転（X軸回転）を反映
	camera_.rotation_.x = ToRadians(rotateAngleX_);
	// 必要に応じて Y 軸回転などもプレイヤーの向きに合わせるとより自然です

	// 5) カメラの座標を更新し、ビュー行列を再計算
	camera_.translation_ = newPos;
	camera_.UpdateViewMatrix();
	camera_.TransferMatrix();
}

void CameraController::Reset() {
	// 追従対象が存在するなら、その位置に即座に移動させたいなどの処理を行う
	if (!target_) {
		return;
	}
	// 例：カメラ位置を目標位置に即座に合わせる
	const WorldTransform& wt = target_->GetWorldTransform();
	Vector3 playerPos = wt.translation_;
	camera_.translation_ = {playerPos.x, playerPos.y + height_, playerPos.z - distance_};
	camera_.rotation_.x = ToRadians(rotateAngleX_);

	camera_.UpdateViewMatrix();
	camera_.TransferMatrix();
}
