#pragma once
#include "3d/Camera.h"
#include "3d/WorldTransform.h"
#include "math/Vector3.h"

class Player;

// カメラをプレイヤーに追従させるクラス
class CameraController {
public:
	CameraController();
	~CameraController();

	// 初期化
	void Initialize();
	// 毎フレーム更新
	void Update();
	// カメラ情報をリセット（必要に応じて）
	void Reset();

	// 追従対象を設定
	void SetTarget(Player* target) { target_ = target; }

	// 現在のカメラオブジェクト取得（描画時などに使用）
	const Camera& GetCamera() const { return camera_; }
	Camera& GetCamera() { return camera_; }

private:
	Player* target_ = nullptr; // カメラが追従する対象（Player）
	Camera camera_;            // カメラ情報（ポジション、回転、ビュー行列など）

	// 追従時に利用する各種パラメータ
	Vector3 targetOffset_ = {0.0f, 1.5f, 0.0f}; // ターゲットから少し上にずらす
	float distance_ = 6.0f;                     // ターゲットとの距離
	float height_ = 3.0f;                       // ターゲットからの垂直オフセット
	float lerpRate_ = 0.1f;                     // 位置補間の速さ

	// 以下は必要に応じて設定
	float rotateAngleX_ = 15.0f; // 見下ろす角度 (度数法)
};
