#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

class CameraController {
public:
	CameraController();
	~CameraController();

	// プレイヤーの位置に基づいてカメラを更新します
	void Update(Camera* camera, const Vector3& playerPosition);

	// カメラのオフセット（プレイヤーからの相対位置）を設定します
	void SetOffset(const Vector3& offset);

	// カメラのピッチ角度（見下ろす角度：度数法）を設定します
	void SetPitch(float pitchDeg);

	void SetYaw(float yawDeg);
	const float GetYaw() { return yawDeg_; }

	void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetTranslate(Vector3 translate) { cameraTranslate = translate; }

private:
	Vector3 offset_; // プレイヤーからのオフセット
	float pitchDeg_; // カメラのピッチ角度（度）
	float yawDeg_;   // カメラのヨー角度（度）

	const WorldTransform* target_ = nullptr;
};